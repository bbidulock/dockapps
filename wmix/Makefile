CC		= gcc
CFLAGS		= -O3 -W -Wall
LDFLAGS		= -L/usr/X11R6/lib
LIBS		= -lXpm -lXext -lX11 -lm
OBJECTS		= misc.o config.o mixer-oss.o ui_x.o mmkeys.o wmix.o

# where to install this program (also for packaging stuff)
DESTDIR		=
PREFIX		= /usr
BINDIR		= $(PREFIX)/bin
MANDIR		= $(PREFIX)/share/man

wmix: $(OBJECTS)
	$(CC) -o $@ $(LDFLAGS) $(OBJECTS) $(LIBS)

clean:
	rm -rf *.o wmix *~

install: wmix
	install -Dm755 wmix $(DESTDIR)$(BINDIR)/wmix
	install -Dm644 wmix.1x $(DESTDIR)$(MANDIR)/man1/wmix.1
