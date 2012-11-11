
ACLOCAL_AMFLAGS = -I m4
WINLIBS=-lshlwapi -lkernel32
AM_CFLAGS = -fPIC -O3 -Wall -Wextra -g

lib_LTLIBRARIES = libchessinterface.la
libchessinterface_la_SOURCES = chessengine.c execproc/execproc.c file/file.c protocolexceptions.c threading/threading.c 
library_includedir=$(includedir)/chessinterface
library_include_HEADERS=chessinterface.h

if WINDOWS_OS
libchessinterface.dll:
	$(CC) -shared $(AM_CFLAGS) \
	$(libchessinterface_la_LDFLAGS) $(LDFLAGS) -o .libs/$@ \
    $(notdir $(patsubst %.c,%.o,$(libchessinterface_la_SOURCES))) \
	$(WINLIBS)
endif

