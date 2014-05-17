dnl Process this file with autoconf to produce a configure script.
AC_INIT(wmfire.c)
AM_INIT_AUTOMAKE(wmfire, 0.0.3.8)
AM_CONFIG_HEADER(config.h)

dnl Checks for programs.
AC_PROG_CC

dnl Checks for libraries.
AC_PATH_X
X11LIBS="-L$x_libraries -I$x_includes -lX11 -lXpm -lXext"
AC_SUBST(X11LIBS)
AC_MSG_CHECKING([to see if we can use X])
AC_TRY_LINK([#include <X11/Xlib.h>],,AC_MSG_RESULT([yes]),AC_MSG_RESULT([uh.. no]); exit 1)

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(unistd.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_PID_T

dnl Checks for library functions.
AC_CHECK_FUNCS(strcspn strdup strspn strstr)

AC_OUTPUT(Makefile)
