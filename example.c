
// This is an example using libchessinterface.
// Install the library first.
// Then compile like this:
//   gcc -o ./example example.c -lchessinterface

#include <stdio.h>

#include <chessinterface/chessinterface.h>

static void engineLoadedCallback(const struct chessengineinfo* info,
void* userdata) {
    printf("loadError: %s\n", info->loadError);
}

static void engineCommunicationLogCallback(int outgoing, const char* line,
void* userdata) {

}

int main(int arc, const char** argv) {
    struct chessinterfaceengine* e = chessinterface_Open(
        "/home/jonas/Develop/ildtiadar2/Ildtiadar2", "",
        NULL, NULL,
        &engineLoadedCallback,
        NULL,
        NULL,
        &engineCommunicationLogCallback,
        NULL
    );
    while (1) { }
}

