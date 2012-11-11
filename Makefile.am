
if WINDOWS_OS
EXAMPLELIBS=-lcomdlg32 -lshlwapi
else
EXAMPLELIBS=
endif

SUBDIRS = src
example$(EXEEXT): example.c
	mkdir -p ./chessinterface
	cp src/chessinterface.h chessinterface/
	$(CC) -I./ -Lsrc/.libs/ -o example$(EXEEXT) example.c -lchessinterface $(EXAMPLELIBS)
	rm -rf ./chessinterface/

# Compile the dll on Windows:
if WINDOWS_OS
all:
	cd src && $(MAKE) libchessinterface.dll
	rm src/.libs/libchessinterface.a
endif
