
// libchessinterface, a library to run chess engines
// Copyright (C) 2012  Jonas Thiem
//
// libchessinterface is free software: you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.
//
// libchessinterface is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with libchessinterface in a file named COPYING.txt.
// If not, see <http://www.gnu.org/licenses/>.

#include "os/os.h"
#ifdef UNIX
#define _GNU_SOURCE
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#endif
#include <string.h>

#include "file/file.h"
#include "threading/threading.h"
#include "execproc/execproc.h"

// Data struct containing info about a running process:
struct process {
    mutex* readdatamutex;
    int* readquitsignal;
    threadinfo* readthreadinfo;
    void (**readcallback)(struct process* p, const char* line,
    void* userdata);
    void** readcallbackuserdata;
#ifdef UNIX
    pid_t pid;
    int stdinpiperead,stdoutpipewrite;
#endif
};

int execproc_Run(const char* file, struct process* p) {
    memset(p, 0, sizeof(*p));
#ifdef UNIX
    // Unix process launcher
    // verify the file exists and is executable:
    if (!file_DoesFileExist(file) || file_IsDirectory(file)) {
        return EXECPROC_ERROR_NOSUCHFILE;
    }

    // do a read test:
    FILE* f = fopen(file, "rb");
    if (!f) {
        return EXECPROC_ERROR_CANNOTOPENFILE;
    }
    fclose(f);

    // prepare pipe:
    int programtouspipe[2];
    int ustoprogrampipe[2];
    if (pipe2(programtouspipe, 0) != 0) {
        return EXECPROC_ERROR_CANNOTRUNFILE;
    }
    if (pipe2(ustoprogrampipe, 0) != 0) {
        close(programtouspipe[0]);
        close(programtouspipe[1]);
        return EXECPROC_ERROR_CANNOTRUNFILE;
    }
    // fork:
    pid_t pid = fork();
    if (pid < 0) {
        close(programtouspipe[0]);
        close(programtouspipe[1]);
        close(ustoprogrampipe[0]);
        close(ustoprogrampipe[1]);
        return EXECPROC_ERROR_CANNOTRUNFILE;
    } else if (pid == 0) {
        // child process, run program here:
        dup2(programtouspipe[1], STDOUT_FILENO);
        dup2(ustoprogrampipe[0], STDIN_FILENO);
        const char* argv[] = { file, NULL };
        if (execv(file, (char* const*) argv) < 0) {
            exit(1);
        }
        exit(0);
    } else {
        // parent process
        p->pid = pid;
        p->stdinpiperead = programtouspipe[0];
        p->stdoutpipewrite = ustoprogrampipe[1];
        return 0;  // return success
    }
#else

#endif
}

void execproc_ReadThread(void* userdata) {
    struct process* p = userdata;
    int readpipe = p->stdinpiperead;
    int* readquitsignal = p->readquitsignal;
    mutex* readdatamutex = p->readdatamutex;
    void (**readcallback)(struct process* p, const char* line,
    void* userdata) = p->readcallback;
    void** readcallbackuserdata = p->readcallbackuserdata;
    while (1) {
        char buf[256] = "";
        while (buf[strlen(buf)-1] != '\n') {
            size_t oldlen = strlen(buf);
            if (oldlen > sizeof(buf)-2) {oldlen = sizeof(buf)-2;}
            int i = recv(readpipe, buf+oldlen, 1, 0);
            mutex_Lock(readdatamutex);
            if (*readquitsignal) {
                // we are asked to terminate, so remove everything:
                free(readquitsignal);
                mutex_Release(readdatamutex);
                mutex_Destroy(readdatamutex);
                free(readcallbackuserdata);
                close(readpipe);
                return;
            }
            if (i <= 0) {
                // read error, process has closed? (or EOF)
                void (*readcallbackvalue)(struct process*, const char*, void*) = *readcallback;
                void* readcallbackuserdatavalue = *readcallbackuserdata;
                mutex_Release(readdatamutex);
                readcallbackvalue(p, NULL, readcallbackuserdatavalue);

                // we can only terminate as soon as we get the quit signal:
                while (1) {
                    usleep(1000 * 1000);  // sleep one second
                    mutex_Lock(readdatamutex);
                    if (*readquitsignal == 1) {
                        free(readquitsignal);
                        mutex_Release(readdatamutex);
                        mutex_Destroy(readdatamutex);
                        free(readcallbackuserdata);
                        close(readpipe);      
                        return;
                    }
                    mutex_Release(readdatamutex);
                }
            } else {
                // data arrived. nullterminate string:
                buf[oldlen+1] = 0;

                // check if we have a complete line:
                if (buf[oldlen+1] == '\n') {
                    // process line:
                    void (*readcallbackvalue)(struct process*, const char*, void*) = *readcallback;
                    void* readcallbackuserdatavalue = *readcallbackuserdata;
                    mutex_Release(readdatamutex);
                    readcallbackvalue(p, buf, readcallbackuserdatavalue);

                    // clear buffer:
                    buf[0] = 0;
                }
            }
        }
    }
}

void execproc_Read(struct process* p, void (*readcallback)(struct process* p,
const char* line, void* userdata), void* userdata) {
    int spawnthread = 1;
    if (!p->readdatamutex) {
        // create various stuff we need for thread communication etc:
        int error = 0;
        p->readdatamutex = mutex_Create();
        if (!p->readdatamutex) {error = 1;}
        p->readquitsignal = malloc(sizeof(*p->readquitsignal));
        if (!p->readquitsignal) {error = 1;}
        p->readthreadinfo = thread_CreateInfo();
        if (!p->readthreadinfo) {error = 1;}
        p->readcallbackuserdata = malloc(sizeof(*p->readcallbackuserdata));
        if (!p->readcallbackuserdata) {error = 1;}
        p->readcallback = malloc(sizeof(*p->readcallback));
        if (!p->readcallback) {error = 1;}
        if (error) {
            // we failed to create something, clean up:
            thread_FreeInfo(p->readthreadinfo);
            mutex_Destroy(p->readdatamutex);
            free(p->readquitsignal);
            free(p->readcallbackuserdata);
            p->readquitsignal = NULL;
            p->readdatamutex = NULL;
            p->readthreadinfo = NULL;
            p->readcallbackuserdata = NULL;
            return;  // nothing senseful we can do
        }
    } else {
        mutex_Lock(p->readdatamutex);
        spawnthread = 0;
    }
    *p->readquitsignal = 0;
    *p->readcallback = readcallback;
    *p->readcallbackuserdata = userdata;
    if (spawnthread) {
        // spawn read thread
        thread_Spawn(p->readthreadinfo, &execproc_ReadThread, p);
    } else {
        // read thread is already running, unlock again:
        mutex_Release(p->readdatamutex);
    }
}

int execproc_Send(struct process* p, const char* line) {
#ifdef UNIX
    if (send(p->stdoutpipewrite, line, strlen(line),
    MSG_NOSIGNAL) < 0) {
        return 0;
    }
    return 1;
#else

#endif
}

void execproc_Close(struct process* p) {
#ifdef UNIX
    close(p->stdinpiperead);
    close(p->stdoutpipewrite);
#endif
    thread_FreeInfo(p->readthreadinfo);

    // tell the read thread to quit:
    mutex_Lock(p->readdatamutex);
    *p->readquitsignal = 1;
    mutex_Release(p->readdatamutex);

    // if the process is still running, stop it:
#ifdef UNIX

#endif

    // free struct:
    free(p);
}


