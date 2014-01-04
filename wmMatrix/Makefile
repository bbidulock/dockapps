CC     = gcc
CFLAGS = -O2 -Wall 
INCDIR = 
DESTDIR=
PREFIX = /usr
LIBDIR = 
LIBS   = -lXpm -lX11 -lXext

OBJS   = wmMatrix.o matrix.o xutils.o yarandom.o

.c.o:
	$(CC) $(CFLAGS) -D$(shell echo `uname -s`) -c $< -o $*.o $(INCDIR)

all:	wmMatrix

wmMatrix.o:	wmMatrix_master.xpm wmMatrix_mask.xbm

wmMatrix:	$(OBJS) 
	$(CC) $(LDFLAGS) -o wmMatrix $^ $(LIBDIR) $(LIBS)

clean:
	for i in $(OBJS) ; do \
		rm -f $$i; \
	done
	rm -f wmMatrix

install:	wmMatrix
	install -Dm755 wmMatrix $(DESTDIR)$(PREFIX)/bin/wmMatrix
	install -Dm644 wmMatrix.1 $(DESTDIR)$(PREFIX)/share/man/man1/wmMatrix.1
