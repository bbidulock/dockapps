#ifndef _SPACE_RGB_BUFFER_H_
#define _SPACE_RGB_BUFFER_H_

#include <algorithm>

#include <gdk-pixbuf/gdk-pixbuf.h>

#include <assert.h>

class Color
{
public:
	unsigned char r;
	unsigned char g;
	unsigned char b;

	Color() { }

	Color(int rr, int gg, int bb) : r(rr), g(gg), b(bb) { }

	Color(double rr, double gg, double bb) 
	{
		r = (unsigned char)(rr * 255);
		g = (unsigned char)(gg * 255);
		b = (unsigned char)(bb * 255);
	}

	unsigned char red()   const { return r; }
	unsigned char green() const { return g; }
	unsigned char blue()  const { return b; }
};

inline Color mix(Color o, Color n, double opacity)
{
	int r = int(o.red()   * (1 - opacity)) + int(n.red()   * opacity);
	int g = int(o.green() * (1 - opacity)) + int(n.green() * opacity);
	int b = int(o.blue()  * (1 - opacity)) + int(n.blue()  * opacity);

	return Color(r, g, b);
}

//--------------------------------------------------------------------

class Rgb_buffer
{
	unsigned char* buf;
	int w;
	int h;

public:
	Rgb_buffer(int w0, int h0) : w(w0), h(h0), buf(new unsigned char[w0 * h0 * 3]) { }

	Rgb_buffer(const Rgb_buffer& b) : w(b.w), h(b.h), buf(new unsigned char[b.w * b.h * 3]) 
	{ 
		std::copy(b.buf, b.buf + w * h * 3, buf);
	}

	Rgb_buffer(GdkPixbuf*);

	~Rgb_buffer() { delete [] buf; }

	unsigned char* buffer() { return buf; }
	const unsigned char* buffer() const { return buf; }
	int width() const { return w; }
	int height() const { return h; }

	void set_pixel(int x, int y, int r, int g, int b)
	{
		assert(x >= 0 && y >= 0 && x < w && y < h);
		unsigned char* c = buf + (x + y * w) * 3;
		*c++ = r;
		*c++ = g;
		*c   = b;
	}
	void set_pixel(int x, int y, Color c)
	{
		set_pixel(x, y, c.red(), c.green(), c.blue());
	}

	Color get_pixel(int x, int y) const 
	{
		assert(x >= 0 && y >= 0 && x < w && y < h);
		unsigned char* c = buf + (x + y * w) * 3;

		int r = *c++;
		int g = *c++;
		int b = *c;

		return Color(r, g, b);
	}

	void clear();
	void clear(int r, int g, int b);
	void clear(Color c) { clear(c.red(), c.green(), c.blue()); }
	
	Rgb_buffer& operator= (const Rgb_buffer& b) 
	{
		if (b.w != w || b.h != h)
		{
			unsigned char* p = new unsigned char[b.w * b.h * 3];
			delete [] buf;
			buf = p;
			w = b.w;
			h = b.h;
		}

		std::copy(b.buf, b.buf + w * h * 3, buf);
	}
};

#endif // _SPACE_RGB_BUFFER_H_
