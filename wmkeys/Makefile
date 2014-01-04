LIBDIR = -L/usr/X11R6/lib
LIBS   = -lXpm -lXext -lX11 -lm
OBJS =	wmkeys.o \
		wmgeneral/wmgeneral.o \
		wmgeneral/misc.o \
		wmgeneral/list.o
DESTDIR =
PREFIX = /usr

.c.o:
	cc -c -O2 -Wall $< -o $*.o

wmkeys: $(OBJS)
	cc -o wmkeys $^ $(LIBDIR) $(LIBS)

clean::
	for i in $(OBJS) ; do \
		rm $$i; \
	done
	rm wmkeys

install:
	install -Dm755 wmkeys $(DESTDIR)$(PREFIX)/bin/wmkeys
