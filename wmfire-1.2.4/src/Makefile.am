bin_PROGRAMS = wmfire

if SESSION
wmfire_SOURCES = wmfire.c \
	session.h session.c \
	flamedefault.h flamecorona.h flameblue.h flamegreen.h
else
wmfire_SOURCES = wmfire.c \
	flamedefault.h flamecorona.h flameblue.h flamegreen.h
endif	 

wmfire_LDADD = @LDFLAGS@

AM_LDFLAGS = -lXext -lX11 \
	-L/usr/X11R6/lib -L/usr/local/lib

INCLUDES = -I/usr/X11R6/include \
	-I/usr/local/include
