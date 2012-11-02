
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

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "threading/threading.h"
#include "chessinterface.h"
#include "execproc/execproc.h"

// This struct will be used by the thread launching the engine:
struct chessinterfaceengine {
    mutex* accesslock;
    struct process* p;
    struct chessengineinfo i;
    char* path;
    char* args;
    char* workingDirectory;
    void (*loadedCallback)(const struct chessengineinfo* info,
      void* userdata);
    void (*communicationLogCallback)(int outgoing, const char* line,
      void* userdata);
    void* userdata;
    int isLoaded;
    int detectionState;
};
#define DETECTIONSTATE_PROBING_UCI 1

static void sendline(struct chessinterfaceengine* cie, const char* line, ...) {
    char linebuf[1024];
    va_list ap;
    va_start(ap, line);
    vsnprintf(linebuf, sizeof(linebuf), line, ap);
    if (strlen(linebuf) < sizeof(linebuf) - 2) {
        strcat(linebuf, "\r\n");
    }
    cie->communicationLogCallback(1, line, cie->userdata);
    execproc_Send(cie->p, linebuf);
}

static void readcallback(struct process* p, const char* line,
void* userdata) {
    struct chessinterfaceengine* cie = userdata;
    mutex_Lock(cie->accesslock);
    if (!line && !cie->isLoaded) {
        struct chessengineinfo* i = &cie->i;
        cie->i.loadError = strdup("Engine shutdown unexpectedly");
        mutex_Release(cie->accesslock);
        cie->loadedCallback(i, cie->userdata);
        return;
    }
    printf("Line received: \"%s\"\n", line);
    mutex_Release(cie->accesslock);
}

// This function will run in the thread that launches the engine:
static void chessinterfacelaunchthread(void* userdata) {
    struct chessinterfaceengine* cie = userdata;
    int i = execproc_Run(cie->path, cie->args,
    cie->workingDirectory, &(cie->p));
    memset(&(cie->i), 0, sizeof(cie->i));
    printf("execproc_Run(%d);\n", i);
    if (i != 0) {
        switch (i) {
        case EXECPROC_ERROR_NOSUCHFILE:
            cie->i.loadError = strdup("No such file");
            break;
        case EXECPROC_ERROR_CANNOTOPENFILE:
            cie->i.loadError = strdup("Cannot open file");
            break;
        case EXECPROC_ERROR_CANNOTRUNFILE:
            cie->i.loadError = strdup("Cannot run file");
            break;
        default:
            cie->i.loadError = strdup("Unknown error");
        }
        cie->loadedCallback(&(cie->i), cie->userdata);
        return;
    }
    if (!sendline(cie, "uci")) {
        cie->i.loadError = strdup("Cannot send data to engine");
        ci->loadedCallback(&(cie->i), cie->userdata);
        return;
    }
    cie->detectionState = DETECTIONSTATE_PROBING_UCI;
    execproc_Read(cie->p, readcallback, cie);
}

struct chessinterfaceengine* chessinterface_Open(const char* path,
const char* args, const char* workingDirectory,
const char* const* protocolOptions,
void (*engineLoadedCallback)(const struct chessengineinfo* info,
  void* userdata),
void (*engineErrorCallback)(const char* error, void* userdata),
void (*engineTalkCallback)(const char* talk, void* userdata),
void (*engineCommunicationLogCallback)(int outgoing, const char* line,
  void* userdata),
void* userdata) {
    struct chessinterfaceengine* cie = malloc(sizeof(*cie));
    if (!cie) {return NULL;}
    memset(cie, 0, sizeof(*cie));
    cie->accesslock = mutex_Create();
    if (!cie->accesslock) {
        free(cie);
        return NULL;
    }
    cie->path = strdup(path);
    if (!cie->path) {
        mutex_Destroy(cie->accesslock);
        free(cie);
        return NULL;
    }
    cie->userdata = userdata;
    cie->loadedCallback = engineLoadedCallback;
    cie->communicationLogCallback = engineCommunicationLogCallback;
    if (args) {
        cie->args = strdup(args);
    }
    if (workingDirectory) {
        cie->workingDirectory = strdup(workingDirectory);
    }

    thread_Spawn(NULL, chessinterfacelaunchthread, cie);
    return cie;
}
