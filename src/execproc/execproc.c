
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
#include <errno.h>
#include <unistd.h>
#endif
#include <string.h>

#include "threading/threading.h"
#include "execproc/execproc.h"


int execproc_Run(const char* file, struct process* p) {
    memset(p, 0, sizeof(*p));
#ifdef UNIX
    // Unix process launcher
    // First, fork:
    pid_t pid = fork();
    if (pid < 0) {
        return EXECPROC_ERROR_CANNOTRUNFILE;
    } else if (pid == 0) {
        // child process, run program here:
        char* argv = { file, NULL };
        if (execvp(file, argv) < 0) {
            exit(1);
        }
        exit(0);
    } else {
        // parent process
    }
#else

#endif
}

void execproc_Read(struct process* p, void (*readcallback)(struct process* p,
const char* line, void* userdata), void* userdata) {

}

int execproc_Send(struct process* p, const char* line) {

}

void execproc_Close(struct process* p) {

}


