CC	= gcc
CFLAGS	= -Wall -O3
LIBDIR 	= -L/usr/X11R6/lib
LIBS   	= -lXpm -lXext -lX11
OBJS 	= wmsm.o ../wmgeneral/wmgeneral.o ../wmgeneral/misc.o ../wmgeneral/list.o
PREFIX	= /usr
DESTDIR =
INSTALL	= /usr/bin/install

wmsm: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBDIR) $(LIBS)

install: wmsm
	install -Dm755 wmsm $(DESTDIR)$(PREFIX)/bin/wmsm

clean:
	rm -f $(OBJS)
	rm -f wmsm
