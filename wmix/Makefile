CC		= gcc
CFLAGS		= -O3 -W -Wall
LDFLAGS		= -L/usr/X11R6/lib
OBJECTS		= misc.o mixer-oss.o ui_x.o wmix.o

# where to install this program (also for packaging stuff)
DESTDIR		=
PREFIX		= /usr

wmix: $(OBJECTS)
	$(CC) -o $@ $(LDFLAGS) $(OBJECTS) -lXpm -lXext -lX11 -lm

clean:
	rm -rf *.o wmix *~

install: wmix
	install -Dm755 wmix $(DESTDIR)$(PREFIX)/bin/wmix
