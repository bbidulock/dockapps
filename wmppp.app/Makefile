LIBDIR = -L/usr/X11R6/lib
LIBS   = -lXpm -lXext -lX11
OBJS  = wmppp.o \
		wmgeneral/wmgeneral.o \
		wmgeneral/misc.o \
		wmgeneral/list.o
DESTDIR =
PREFIX = /usr
BINDIR = $(PREFIX)/bin
LIBDIR = $(PREFIX)/lib/wmppp.app
SYSCONFDIR = /etc/wmppp
MANDIR = $(PREFIX)/share/man/man1

all: wmppp getmodemspeed

.c.o:
	$(CC) -g $(CPPFLAGS) $(CFLAGS) -Wall -c -o $@ $<

wmppp: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBDIR) $(LIBS)

getmodemspeed: getmodemspeed.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBDIR) $(LIBS)

clean:
	for i in $(OBJS) ; do \
		rm -f $$i; \
	done
	rm -f getmodemspeed.o
	rm -f wmppp getmodemspeed

install: wmppp getmodemspeed
	install -Dm755 wmppp $(DESTDIR)$(PREFIX)/bin/wmppp
	install -Dm755 getmodemspeed $(DESTDIR)$(PREFIX)/bin/getmodemspeed
	install -Dm644 system.wmppprc $(DESTDIR)$(SYSCONFDIR)/system.wmppprc
	install -Dm644 user.wmppprc $(DESTDIR)$(SYSCONFDIR)/user.wmppprc
