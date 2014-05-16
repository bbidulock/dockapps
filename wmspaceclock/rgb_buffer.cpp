
#include "rgb_buffer.h"

#include <algorithm>

#include <gdk-pixbuf/gdk-pixbuf.h>

void Rgb_buffer::clear()
{
    std::fill_n(buf, w * h * 3, 0);
}

void Rgb_buffer::clear(int r, int g, int b)
{
    for (int i = 0; i < w*h; ++i)
    {
		buf[i * 3 + 0] = r;
		buf[i * 3 + 1] = g;
		buf[i * 3 + 2] = b;
    }
}

Rgb_buffer::Rgb_buffer(GdkPixbuf* pixbuf)
{
    w = gdk_pixbuf_get_width(pixbuf);
    h = gdk_pixbuf_get_height(pixbuf);

    buf = new unsigned char[w * h * 3];

    bool alpha = gdk_pixbuf_get_has_alpha(pixbuf);
    guchar* mem = gdk_pixbuf_get_pixels(pixbuf);
    int s = gdk_pixbuf_get_rowstride(pixbuf);
    for (int y = 0; y < h; ++y)
    {
		if (alpha)
		{
			guchar *ps = mem + y * s;
			unsigned char* pd = buf + y * w * 3;

			for (int n = w; n != 0; --n)
			{
			*pd++ = *ps++;
			*pd++ = *ps++;
			*pd++ = *ps++;
			ps++;
			}
		}
		else
		{
			std::copy(mem + y * s, mem + y * s + w * 3, buf + y * w * 3);
		}
    }

    gdk_pixbuf_unref(pixbuf);
}

