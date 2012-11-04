
// This is an example using libchessinterface.
// Install the library first.
// Then compile like this:
//   gcc -o ./example example.c -lchessinterface

#include <stdio.h>

#include <chessinterface/chessinterface.h>

static volatile int probed = 0;

static void engineLoadedCallback(struct chessinterfaceengine* engine,
const struct chessengineinfo* info,
void* userdata) {
    probed = 1;
    if (info->loadError) {
        printf("Failed to load engine.\n");
        return;
    }
    printf("Engine protocol: %s\n", info->protocolType);
    const char* const* p = info->engineInfo;
    while (*p) {
        printf("Engine info value: %s: %s\n", *p, *(p+1));
        p += 2;
    }
    chessinterface_Close(engine);
}

static void engineCommunicationLogCallback(
struct chessinterfaceengine* engine, int outgoing, const char* line,
void* userdata) {
    printf("Communication (%d): %s\n", outgoing, line);
}

int main(int argc, const char** argv) {
    if (argc < 2) {
        printf("Please specify an engine to probe.\n");
        return 1;
    }

    struct chessinterfaceengine* e = chessinterface_Open(
        argv[1], "",
        NULL, NULL,
        &engineLoadedCallback,
        NULL,
        NULL,
        &engineCommunicationLogCallback,
        NULL,
        NULL
    );
    while (!probed) {
        
    }
    return 0;
}

