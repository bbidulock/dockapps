/* spaceclock - Yet Another Waste of CPU Cycles!
 * Copyright (C) 2002 Attila Tajti <attis@spacehawks.hu>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You did not receive a copy of the GNU General Public License along with
 * this program; chances are, you already have at least 10 copies of this
 * license somewhere on your system.  If you still want it, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 *
 * History :
 *
 * 2002-10-01: Initial version
 *
 */

#include <cstring>
#include <exception>
#include <unistd.h>

/* x11 includes */
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <X11/Xresource.h>

#include "rgb_buffer.h"
#include "dockapp.h"

//--------------------------------------------------------------------

/* This function makes the dockapp window */
DockApp::DockApp(int argc, char** argv, const char *name, GdkPixbuf* master) throw (wm_error)
{
    /* set attributes for dockapp window */
    GdkWindowAttr attr;
    std::memset(&attr, 0, sizeof(GdkWindowAttr));

    attr.width = 64;
    attr.height = 64;
    attr.title = const_cast<char*>(name);
    attr.event_mask = GDK_BUTTON_PRESS_MASK | \
		    GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK;
    attr.wclass = GDK_INPUT_OUTPUT;
    attr.visual = gdk_visual_get_system();
    attr.colormap = gdk_colormap_get_system();
    attr.wmclass_name = const_cast<char*>(name);
    attr.wmclass_class = const_cast<char*>(name);
    attr.window_type = GDK_WINDOW_TOPLEVEL;

    /* make a copy for the iconwin - parameters are the same */
    GdkWindowAttr attri;
    std::memcpy(&attri, &attr, sizeof(GdkWindowAttr));

    XSizeHints sizehints;
    sizehints.flags = USSize;
    sizehints.width = 64;
    sizehints.height = 64;

    // create window and icon window
    win = gdk_window_new(0, &attr,
			    GDK_WA_TITLE | GDK_WA_WMCLASS |
			    GDK_WA_VISUAL | GDK_WA_COLORMAP);

    if (!win) throw wm_error();

    iconwin = gdk_window_new(win, &attri,
				GDK_WA_TITLE | GDK_WA_WMCLASS);

    if (!iconwin) throw wm_error();

    // set window hints
    Window xwin = GDK_WINDOW_XWINDOW(win);
    Window xiconwin = GDK_WINDOW_XWINDOW(iconwin);
    XSetWMNormalHints(GDK_WINDOW_XDISPLAY(win), xwin, &sizehints);
    XSetCommand(GDK_WINDOW_XDISPLAY(win), xwin, argv, argc);

    XWMHints wmhints;

    wmhints.initial_state = WithdrawnState;
    wmhints.icon_window = xiconwin;
    wmhints.icon_x = 0;
    wmhints.icon_y = 0;
    wmhints.window_group = xwin;
    wmhints.flags =
		StateHint | IconWindowHint | IconPositionHint | WindowGroupHint;
    XSetWMHints(GDK_WINDOW_XDISPLAY(win), xwin, &wmhints);

    // create the graphic context and the pixmap
    gc = gdk_gc_new(win);

    //pixmap = gdk_pixmap_create_from_xpm_d(win, &(mask), 0, master_xpm);

    gdk_pixbuf_render_pixmap_and_mask(master, &pixmap, &mask, 128);

    pixmap = gdk_pixmap_new(win, attr.width, attr.height, -1);
    //gdk_pixbuf_render_to_drawable(master, pixmap, gc,
	    //0, 0, 0, 0, attr.width, attr.height, GDK_RGB_DITHER_NONE, 0, 0);

    gdk_window_shape_combine_mask(win, mask, 0, 0);
    gdk_window_shape_combine_mask(iconwin, mask, 0, 0);

    gdk_window_set_back_pixmap(win, pixmap, false);
    gdk_window_set_back_pixmap(iconwin, pixmap, false);

    gdk_window_show(win);

    sleep_usec = 100000;
}

void DockApp::run()
{
    while (true)
    {
	while (gdk_events_pending())
	{
	    GdkEvent* event = gdk_event_get();
	    if (event)
	    {
		switch (event->type)
		{
		    case GDK_DESTROY:
			    gdk_exit(0);
			    return;
			    /*
			    exit(0);
			    break;
		    case GDK_BUTTON_PRESS:
			    // printf("button press\n");
			    break;
		    case GDK_ENTER_NOTIFY:
			    proximity = 1;
			    for (ch = 0; ch < NRFISH; ch++)
			    bm.fishes[ch].speed += (rand() % 2) + 1;
			    break;
		    case GDK_LEAVE_NOTIFY:
			    proximity = 0;
			    // get fish moving again, but slowly - checking out
			    for (ch = 0; ch < NRFISH; ch++)
			    bm.fishes[ch].speed = 1;
			    break;
			    */
		}
	    }
	}

	usleep(sleep_usec);

	render();
    }
}

void DockApp::render(bool)
{
}

void DockApp::draw(int x, int y, const Rgb_buffer* b)
{
    gdk_draw_rgb_image(iconwin, gc, x, y, b->width(), b->height(),
	GDK_RGB_DITHER_NONE, const_cast<unsigned char*>(b->buffer()), b->width() * 3);
}

