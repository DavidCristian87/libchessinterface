
ACLOCAL_AMFLAGS = -I m4

lib_LTLIBRARIES = libchessinterface.la
libchessinterface_la_SOURCES = chessengine.c execproc/execproc.c file/file.c protocolexceptions.c threading/threading.c 
library_includedir=$(includedir)/chessinterface
library_include_HEADERS=chessinterface.h

AM_CFLAGS = -fno-PIC -O3 -Wall -Wextra -g

