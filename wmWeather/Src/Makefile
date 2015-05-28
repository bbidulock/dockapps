CC     = gcc
CFLAGS = -O2 -Wall
INCDIR = -I/usr/X11R6/include/X11 -I/usr/X11R6/include
DESTDIR=
PREFIX = /usr
LIBDIR = -L/usr/X11R6/lib
# for linux
LIBS   = -lXpm -lX11 -lXext
# for Solaris
# LIBS   = -lXpm -lX11 -lXext -lsocket
OBJS   = wmWeather.o \
         xutils.o


.c.o:
	$(CC) $(CFLAGS) -D$(shell echo `uname -s`) -c $< -o $*.o $(INCDIR)


all:	wmWeather.o wmWeather

wmWeather.o: wmWeather_master.xpm wmWeather_mask.xbm
wmWeather:	$(OBJS)
	$(CC) $(CFLAGS) $(SYSTEM) -o wmWeather $^ $(INCDIR) $(LIBDIR) $(LIBS)

clean:
	for i in $(OBJS) ; do \
		rm -f $$i; \
	done
	rm -f wmWeather

install:: wmWeather
	install -Dm755 wmWeather $(DESTDIR)$(PREFIX)/bin/wmWeather
	install -Dm755 GrabWeather $(DESTDIR)$(PREFIX)/bin/GrabWeather
	install -Dm644 wmWeather.1 $(DESTDIR)$(PREFIX)/share/man/man1/wmWeather.1

