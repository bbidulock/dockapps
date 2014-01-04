# wmckgmail 1.1 : A dockapp to monitor the number of unread mails in a gmail inbox
# written by    : Sylvain Tremblay <stremblay@gmail.com>
# release date  : September 09, 2006

LIBDIR = -L/usr/X11R6/lib
LIBS   = -lXpm -lXext -lX11 -lm
FLAGS = -O2
PREFIX = /usr/local
OBJS =	wmckgmail.o \
		../wmgeneral/wmgeneral.o \
		../wmgeneral/misc.o \
		../wmgeneral/list.o


.c.o:
	cc -I/usr/X11R6/share/include $(FLAGS) -D$(LANG) -c -Wall $< -o $*.o

wmckgmail: $(OBJS)
	cc $(FLAGS) -D$(LANG) -o wmckgmail $^ -lXext $(LIBDIR) $(LIBS)

all:: wmckgmail

clean::
	for i in $(OBJS) ; do \
		rm -f $$i ; \
	done
	rm -f wmckgmail
	rm -f *~

install:: wmckgmail
	install -Dm755 wmckgmail $(DESTDIR)$(PREFIX)/bin/wmckgmail
	install -Dm644 wmckgmail.1 $(DESTDIR)$(PREFIX)/share/man/man1/wmckgmail.1
