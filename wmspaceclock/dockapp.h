#ifndef _SPACE_DOCKAPP_H_
#define _SPACE_DOCKAPP_H_

#include <exception>

#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

class Rgb_buffer;

class DockApp
{
    Display *display;		/* X11 display */
    GdkWindow *win;			/* main window */
    GdkWindow *iconwin;		/* icon window */
    GdkGC *gc;				/* drawing GC */
    GdkPixmap *pixmap;		/* main dockapp pixmap */
    GdkBitmap *mask;		/* dockapp mask */

    unsigned sleep_usec;

public:
    class wm_error : public std::exception { };

    //DockApp(const char*) throw (wm_error);
    DockApp(int ac, char** av, const char* n, GdkPixbuf* p) throw (wm_error);
    virtual ~DockApp() { }
    void run();

    void set_sleep_time(unsigned t) { sleep_usec = t; }
    unsigned sleep_time() const { return sleep_usec; }

    virtual void render(bool force = false);
    virtual void draw(int, int, const Rgb_buffer*);
};

#endif // _SPACE_DOCKAPP_H_
