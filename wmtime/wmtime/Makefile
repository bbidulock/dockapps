LIBDIR = -L/usr/X11R6/lib
LIBS   = -lXpm -lXext -lX11 -lm
OBJS =	wmtime.o \
		../wmgeneral/wmgeneral.o \
		../wmgeneral/misc.o \
		../wmgeneral/list.o
XPMS = wmtime-master.xpm wmtime-mask.xbm
PREFIX = /usr
DESTDIR = 

CC = gcc
CFLAGS += -O2

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) -c -Wall $< -o $*.o

wmtime: $(OBJS) $(XPMS)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) -o wmtime $(OBJS) $(LIBDIR) $(LIBS)

install: wmtime
	install -Dm755 wmtime $(DESTDIR)$(PREFIX)/bin/wmtime
	install -Dm644 wmtime.1 $(DESTDIR)$(PREFIX)/share/man/man1/wmtime.1

clean::
	for i in $(OBJS) ; do \
		rm $$i; \
	done
	rm wmtime
