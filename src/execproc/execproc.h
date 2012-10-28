
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

#ifndef EXECPROC_H_
#define EXECPROC_H_

#include "os/os.h"

// Data struct containing info about a running external process:
struct process {
    
};

// Run an external process, and store the information about the running
// process in the process struct:
int execproc_Run(const char* file, struct process* p);
// Returns 0 on success, or one of the following error codes:
#define EXECPROC_ERROR_NOSUCHFILE -1  // file path doesn't lead to a file
#define EXECPROC_ERROR_CANNOTOPENFILE -2  // cannot open the file
#define EXECPROC_ERROR_CANNOTRUNFILE -3  // the file cannot be run
// In case of an error, the contents of struct process are undefined
// and you can ignore them. In case of success, you can now use
// various functions like execproc_Read to communicate with the process,
// and you must close the process with execproc_Close when finished.

// Begin reading from an opened process (from its stdout/stderr).
// Call this with a callback function of your choice,
// and your callback will be called as soon as any data is available
// (FROM ANOTHER THREAD!).
// If you wish to stop reading, supply NULL as callback (or simply
// close the whole process with execproc_Close);
void execproc_Read(struct process* p, void (*readcallback)(struct process* p,
const char* line, void* userdata), void* userdata);
// Your callback gets passed the process it has received data from,
// the line it received, and the userdata you supply for it.
//
// You will always get one full line, and the line will always be
// terminated with a \n character.
// Empty lines (or lines just containing space chars or tabulator
// characters) will be dropped.
//
// IMPORTANT: You may get NULL as line. In that case, the process has
// shutdown. Use execproc_Close on the process struct to clean up!

// Send a line to an opened process (to its stdin).
// You may also send multiple lines at once.
// execproc_Send may strip empty lines.
int execproc_Send(struct process* p, const char* line);
// Returns 1 on success, 0 on failure: on failure, the process has
// closed down and you must use execproc_Close to clean things up.

// Close down a process:
void execproc_Close(struct process* p);
// This must be used either after execproc_Send or execproc_Read
// gave you failure (which means the process has closed by itself),
// or when you wish the process to be closed on your behalf.
// If you want to close the process on your behalf and you just
// used execproc_Send to send a command to it to indicate it should
// close, you should probably wait a second or so before calling
// execproc_Close which will force it to close down.
// On Windows, the process is instantly terminated.
// On Linux, SIGTERM is sent and after 2 seconds, SIGKILL will be sent
// (this happens in another thread, so this call won't hang your
// program for 2 seconds before returning).

#endif  // EXECPROC_H_

