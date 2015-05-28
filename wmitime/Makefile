LIBDIR = -L/usr/X11R6/lib
LIBS   = -lXpm -lXext -lX11 -lm
CFLAGS = -O2 -Wall
OBJS = wmitime.o wmgeneral/wmgeneral.o
PREFIX = /usr
BINDIR = $(PREFIX)/bin
MANDIR = $(PREFIX)/share/man
DESKTOPDIR = $(PREFIX)/share/applications

.c.o:
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $*.o

wmitime: $(OBJS)
	$(CC) $(LDFLAGS) -o wmitime $^ $(LIBDIR) $(LIBS)

all:: wmtime

clean::
	for i in $(OBJS) ; do \
		rm -f $$i ; \
	done
	rm -f wmitime wmitime.desktop
	rm -f *~

wmitime.desktop:
	sed "s|@BINDIR@|$(BINDIR)|" wmitime.desktop.in > $@

install:: wmitime wmitime.desktop
	install -Dm755 wmitime $(DESTDIR)$(BINDIR)/wmitime
	install -Dm644 wmitime.1 $(DESTDIR)$(MANDIR)/man1/wmitime.1
	install -DM644 wmitime.desktop $(DESTDIR)$(DESKTOPDIR)/wmitime.desktop
