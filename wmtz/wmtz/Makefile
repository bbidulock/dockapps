CC = gcc
LIBDIR = -L/usr/X11R6/lib
LIBS   = -lXpm -lXext -lX11 -lm
FLAGS = -O2
OBJS =	wmtz.o \
		../wmgeneral/wmgeneral.o \
		../wmgeneral/misc.o \
		../wmgeneral/list.o
PREFIX = /usr
DESTDIR =

.c.o:
	$(CC) -I/usr/X11R6/include $(FLAGS) -c -Wall $< -o $*.o

wmtz: $(OBJS)
	$(CC) $(FLAGS) -o wmtz $^ -lXext $(LIBDIR) $(LIBS)

all:: wmtz

clean::
	for i in $(OBJS) ; do \
		rm -f $$i ; \
	done
	rm -f wmtz
	rm -f *~

install:: wmtz
	install -Dm755 wmtz $(DESTDIR)$(PREFIX)/bin/wmtz
	install -Dm644 wmtz.1 $(DESTDIR)$(PREFIX)/share/man/man1/wmtz.1
	install -Dm644 wmtzrc $(DESTDIR)/etc/wmtzrc

