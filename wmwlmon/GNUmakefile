# $Id: GNUmakefile,v 1.2 2005/10/20 15:25:28 hacki Exp $

CCFLAGS = -O2 -pipe -Wall -Werror -Wstrict-prototypes -ansi -I/usr/X11R6/include -c
DESTDIR =
PREFIX = /usr

wmwlmon:	wmwlmon.o wl.o xutils.o
		cc -o wmwlmon wmwlmon.o wl.o xutils.o -L/usr/X11R6/lib -lXpm -lX11 -lXext -lm
		strip wmwlmon

wmwlmon.o:	wmwlmon.c
		cc ${CCFLAGS} wmwlmon.c

wl.o:		wl.c
		cc ${CCFLAGS} wl.c

xutils.o:	xutils.c
		cc ${CCFLAGS} xutils.c

clean:
		rm -f wmwlmon *.o

install:
		install -Dm755 wmwlmon $(DESTDIR)$(PREFIX)/bin/wmwlmon
		install -Dm644 wmwlmon.1 $(DESTDIR)$(PREFIX)/share/man/man1/wmwlmon.1
