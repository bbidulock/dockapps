CC     = gcc
CFLAGS = -O2 -Wall
INCDIR = -I/usr/X11R6/include/X11 -I/usr/X11R6/include
DESTDIR= 
PREFIX = /usr
LIBDIR = -L/usr/X11R6/lib
BINDIR = $(PREFIX)/bin
MANDIR = $(PREFIX)/share/man

# for Linux
LIBS   = -lXpm -lX11 -lXext

# for Solaris
# LIBS   = -lXpm -lX11 -lXext -lsocket -lnsl

OBJS   = wmCalClock.o \
         xutils.o


.c.o:
	$(CC) $(CFLAGS) -D$(shell echo `uname -s`) -c $< -o $*.o $(INCDIR)


all:	wmCalClock.o wmCalClock

wmCalClock.o: wmCalClock_master.xpm wmCalClock_mask.xbm
wmCalClock:	$(OBJS)
	$(CC) $(COPTS) $(SYSTEM) -o wmCalClock $^ $(INCDIR) $(LIBDIR) $(LIBS)

clean:
	for i in $(OBJS) ; do \
		rm -f $$i; \
	done
	rm -f wmCalClock

install:: wmCalClock
	install -Dm0755 wmCalClock $(DESTDIR)$(BINDIR)/wmCalClock
	install -Dm0644 wmCalClock.1 $(DESTDIR)$(MANDIR)/man1/wmCalClock.1

