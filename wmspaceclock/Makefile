DESTDIR =
PREFIX = $(DESTDIR)/usr/local

CPPFLAGS = -I -g3 -DINSTALL_PREFIX=\"$(PREFIX)\"

LDFLAGS =
X_CPPFLAGS = `gtk-config --cflags` `gdk-pixbuf-config --cflags`
X_LDFLAGS = `gtk-config --libs` `gdk-pixbuf-config --libs`

CPPFLAGS += $(X_CPPFLAGS)
LDFLAGS += $(X_LDFLAGS)

# optional settings for STLport
# note: STLport will speed up spaceclock quite a bit
-include configure.mk

PRG = spaceclock
PRGVER = 1.2d
CPPFLAGS += -DPRGVER=\"$(PRGVER)\"

PARTS = dockapp clockapp config rgb_buffer raster scanliner raster_bad

HDRS = $(addsuffix .h   ,$(PARTS) $(PRG) util)
SRCS = $(addsuffix .cpp ,$(PARTS) $(PRG))
OBJS = $(addsuffix .o   ,$(PARTS) $(PRG))

DATAFILES = $(wildcard *.theme) $(wildcard *.hands)
DISTFILES = $(HDRS) $(SRCS) master.xpm index.html \
    configure Makefile ChangeLog README COPYING $(DATAFILES)

#all: $(PRG)

all: $(PRG) README

$(PRG): $(OBJS) configure.mk
	$(CXX) -o $(PRG) $(LDFLAGS) $(OBJS)

depend:
	$(CXX) -MM $(CPPFLAGS) $(SRCS) 1>.depend

clean:
	@-rm $(OBJS) $(PRG) configure.mk

tags: $(SRCS) $(HDRS)
	ctags $(SRCS) $(HDRS)

configure.mk:
	./configure

README: index.html
	lynx -dump index.html >README

.PHONY: tbz
tbz:
	-rm -rf tmp
	mkdir tmp tmp/$(PRG)
	cp -fr $(DISTFILES) pixmaps tmp/$(PRG)
	cd tmp && tar -cjf ../$(PRG)-$(PRGVER).tar.bz2 $(PRG)
	rm -rf tmp

sf:
	# upload project to my sf page
	scp index.html attis74@wmspaceclock.sourceforge.net:/home/groups/w/wm/wmspaceclock/htdocs/
	@echo "DONT FORGET TO CREATE/EDIT PROJECT FILE RELEASES!!!"


install:
	install -s -m 755 $(PRG) $(PREFIX)/bin
	install -d $(PREFIX)/share/$(PRG)
	install -d $(PREFIX)/share/$(PRG)/pixmaps
	install -m 644 pixmaps/* $(PREFIX)/share/$(PRG)/pixmaps
	install -m 644 $(DATAFILES) $(PREFIX)/share/$(PRG)

#
# include dependency files if they exist
#
ifneq ($(wildcard .depend),)
include .depend
endif

# vim:tabstop=8
