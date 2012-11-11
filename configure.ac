
# libchessinterface, a library to run chess engines
# Copyright (C) 2012  Jonas Thiem
#
# libchessinterface is free software: you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# libchessinterface is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with libchessinterface in a file named COPYING.txt.
# If not, see <http://www.gnu.org/licenses/>.

AC_PREREQ([2.60])

AC_INIT([libchessinterface], [1.0], [jonasthiem@googlemail.com])
LT_INIT([dlopen win32-dll])

AM_INIT_AUTOMAKE([foreign])
AC_PROG_CC
AC_PROG_CXX
AC_PROG_MAKE_SET
AC_PROG_OBJC
AC_CANONICAL_HOST
AC_DISABLE_STATIC
AC_ENABLE_SHARED
AC_CONFIG_MACRO_DIR([m4])
AC_CHECK_TOOL(AR, ar)
if [ test -z "$AR" ]; then
    AC_MSG_ERROR([No ar available. Please install ar.])
    exit 1
fi

AC_PREFIX_DEFAULT([/usr])

AS_IF([test "x$libdir" = "x\${exec_prefix}/lib"],[
    # Since default libdir wasn't changed, set to lib64 if on 64bit
    ARCH="`uname -m`"
    AS_IF([test "x$ARCH" = xx86_64], [
        libdir="\${exec_prefix}/lib64"
    ])
])

# Check whether target os is windows:
maybeos=`echo $EXEEXT | grep exe`
AM_CONDITIONAL(WINDOWS_OS, [test -n "$maybeos"])

# OS dependant things:
AS_IF([test -z "$maybeos"],[
# UNIX (Linux/Mac OS X)
    AC_MSG_NOTICE([Trying to compile for a Unix target.]) 

    MACTEST=`uname -a | grep Darwin`
    AS_IF([test -z "$MACTEST"],[
        # Non-Mac Unix:
        LIBS="-pthread -ldl -lm -lrt"
    ],[
        # Mac OS X:
        LIBS="$LIBS -lm -pthread"
    ])

    # Checks for header files and functions:
    AC_CHECK_LIB([dl], [dlopen],, AC_MSG_ERROR(Cannot find all the necessary functions. Try to install the necessary libraries and headers))
    AC_CHECK_HEADERS([malloc.h])
    AC_CHECK_FUNCS([snprintf strncat],,AC_MSG_ERROR(Cannot find all the necessary functions. Try to install the necessary libraries and headers))
    AC_FUNC_MALLOC(,,AC_MSG_ERROR(Cannot find all the necessary functions. Try to install the necessary libraries and headers))
    AC_FUNC_REALLOC(,,AC_MSG_ERROR(Cannot find all the necessary functions. Try to install the necessary libraries and headers))

    TGHOST="$host_alias"
    AC_SUBST(TGHOST)
],[
# WINDOWS
    AC_MSG_NOTICE([Attempting to compile for a Windows target.]) 

    # Check if we are using mingw:
    compileros=`echo $CC | grep mingw`
    AS_IF([test -z "$compileros"], [
    AC_WARN([You have chosen a mingw target but apparently the compiler is not a mingw cross-compiler. Please fix this if you are on Unix trying to cross-compile for Windows. IF YOU ARE ON WINDOWS RIGHT NOW, EVERYTHING IS FINE!])
    ])

    TGHOST="$host_alias"
    AC_SUBST(TGHOST)
    
    CFLAGS="$CFLAGS -static-libgcc -mwindows -mthreads"
    LIBS="$LIBS -lws2_32 -lwinmm"

    AC_CHECK_HEADERS([windows.h string.h stdio.h unistd.h],,AC_MSG_ERROR(Cannot find all the necessary headers. Try to install the necessary libraries and headers))

])

AC_SUBST(EXEEXT)

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT

