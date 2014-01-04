LIBDIR = -L/usr/X11R6/lib
LIBS   = -lXpm -lXext -lX11
CFLAGS = -O2
OBJS  = wmppp.o \
		../wmgeneral/wmgeneral.o \
		../wmgeneral/misc.o \
		../wmgeneral/list.o
DESTDIR =
PREFIX = /usr

.c.o:
	cc -g -c $(CFLAGS) -Wall $< -o $*.o

all: wmppp getmodemspeed

wmppp: $(OBJS)
	cc -o wmppp $^ -lXext $(LIBDIR) $(LIBS)

clean:
	for i in $(OBJS) ; do \
		rm $$i; \
	done
	rm wmppp getmodemspeed

install: wmppp getmodemspeed
	install -Dm755 wmppp $(DESTDIR)$(PREFIX)/bin/wmppp
	install -Dm755 getmodemspeed $(DESTDIR)$(PREFIX)/bin/getmodemspeed
	install -Dm644 system.wmppprc $(DESTDIR)/etc/wmppp/system.wmppprc
	install -Dm644 user.wmppprc $(DESTDIR)/etc/wmppp/user.wmppprc
