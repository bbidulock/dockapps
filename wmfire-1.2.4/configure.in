AC_INIT(src/wmfire.c)

AM_INIT_AUTOMAKE(wmfire, 1.2.4)

AC_PROG_AWK
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S

AC_PATH_X
AC_CHECK_LIB(X11, main)
AC_CHECK_LIB(Xext, main)
AC_CHECK_LIB(m,sincos)

AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS(unistd.h)

AC_C_CONST

AC_ARG_ENABLE(header, [
Wmfire options:
])

AC_ARG_ENABLE(session,
[  --enable-session                Compile with session management],
       session=on, session=off)

if test "x$session" = "xon" ; then
  AC_DEFINE(SESSION)
  LDFLAGS="$LDFLAGS -lSM -lICE"
fi
AM_CONDITIONAL(SESSION, test "x$session" = "xon")

AC_PATH_PROG(config, pkg-config)

AC_CACHE_CHECK([for gdk2 includes], GDKINCS, [GDKINCS=`$config --cflags gtk+-2.0`])
AC_CACHE_CHECK([for gdk2 libs], GDKLIBS, [GDKLIBS=`$config --libs gtk+-2.0 | sed -e "s/-lgtk-x11-2.0//g" -e "s/-latk-1.0//g"`])

AC_CACHE_CHECK([for libgtop2 includes], GTOPINCS, [GTOPINCS=`$config --cflags libgtop-2.0`])
AC_CACHE_CHECK([for libgtop2 libs], GTOPLIBS, [GTOPLIBS=`$config --libs libgtop-2.0`])

CFLAGS="$CFLAGS $GDKINCS $GTOPINCS"
LDFLAGS="$LDFLAGS $GDKLIBS $GTOPLIBS"

AC_CHECK_FUNCS(strstr)

AC_OUTPUT(Makefile src/Makefile)
