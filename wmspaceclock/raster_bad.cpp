
#include <vector>
#include <algorithm>
#include <iterator>

#include <iomanip>
#include <iostream>

#include "raster_bad.h"
#include "rgb_buffer.h"
#include "util.h"

class Subpixel_mask
{
	unsigned* buf;
	int w, h;

	int xd;
	int yd;

public:
	Subpixel_mask(int, int, int, int);
	~Subpixel_mask();

	struct pos
	{
		int x, y;

		pos(int x0, int y0) : x(x0), y(y0) { }
	};

	int alpha_max() const { return xd * yd; }
	int xdiv() const { return xd; }
	int ydiv() const { return yd; }

	int alpha_at(int, int) const;
	void clear_alpha(int, int);
	
	void toggle_pixel(int, int);
	void set_pixel(int, int);
	void set_pixel(int, int, bool);

	int width() const { return w; }
	int height() const { return h; }

	pos vertex_pos(const Vertex& v) const;
};

Subpixel_mask::Subpixel_mask(int w0, int h0, int xd0, int yd0) 
	: w(w0), h(h0), xd(xd0), yd(yd0), buf(new unsigned[w0*h0])
{
}

Subpixel_mask::~Subpixel_mask()
{
	delete [] buf;
}

void Subpixel_mask::clear_alpha(int x, int y) 
{
	buf[x + y * w] = 0;
}

int Subpixel_mask::alpha_at(int x, int y) const
{
	unsigned v = buf[x + y * w];

	// handle special cases
	if (v ==  0) return 0;
	if (v == ~0) return xd*yd;

	int n = 0;
	for (int i = 0; i < xd * yd; ++i)
	{
		if (v & (1 << i)) ++n;
	}

	return n;
}

void Subpixel_mask::toggle_pixel(int x, int y)
{
	int xh = x / xd;
	int xl = x % xd;
	int yh = y / yd;
	int yl = y % yd;

	unsigned* p = buf + xh + yh * w;

	*p ^= 1 << (xl + yl * xd);
}

void Subpixel_mask::set_pixel(int x, int y)
{
	int xh = x / xd;
	int xl = x % xd;
	int yh = y / yd;
	int yl = y % yd;

	unsigned* p = buf + xh + yh * w;

	*p |= 1 << (xl + yl * xd);
}

void Subpixel_mask::set_pixel(int x, int y, bool on)
{
	int xh = x / xd;
	int xl = x % xd;
	int yh = y / yd;
	int yl = y % yd;

	unsigned* p = buf + xh + yh * w;

	if (on)
	{
		*p |= 1 << (xl + yl * xd);
	}
	else
	{
		*p &= ~(1 << (xl + yl * xd));
	}
}

Subpixel_mask::pos Subpixel_mask::vertex_pos(const Vertex& v) const
{
	return pos((int)v.x * xdiv(), (int)v.y * ydiv());
}

//--------------------------------------------------------------------

using std::vector;

class Simple_aa_rasterizer : public Rasterizer, protected Scanline_generator
{
	Rgb_buffer* rgb;
	Subpixel_mask m;
	bool pixel_on_flag; // drawing or erasing

	void apply_mask(Color c);
	void mark_pixel(int, int);
	virtual void rasterize_scanline(int, xit, xit);

	bool need_first_vertex;
	Vertex first_vertex;

public:
	Simple_aa_rasterizer(Rgb_buffer*);

	virtual void begin_shape();
	virtual void begin_pgon();
	virtual void vertex(const Vertex&);
	virtual void end_pgon(bool);
	virtual void end_shape(Color);
};

Simple_aa_rasterizer::Simple_aa_rasterizer(Rgb_buffer* b) :
	Scanline_generator(b->width(), b->height(), 8, 4), 
	rgb(b), 
	m(b->width(), b->height(), 8, 4)
{
}

//
// .....**************>*****************>.*********> ..................
//      0              1                 34         5
//                     2
//
// .....***************>...............................................
//      0               1         2             4    5
//                                3
//

void Simple_aa_rasterizer::rasterize_scanline(int y, 
		Scanline_generator::xit begin, Scanline_generator::xit end)
{
	xit i = begin;
	if ((end - begin) % 1) --end;

#ifdef DBG
	std::cout << std::setw(2) << y << ": ";
#endif
	while (i != end)
	{
		int ax = *i++;
		int bx = *i++;
#ifdef DBG
		std::cout << ax << "," << bx << " ";
#endif

		for (int x = ax; x < bx; ++x)
		{
			m.set_pixel(x, y, pixel_on_flag);
		}
	}
#ifdef DBG
	std::cout << std::endl;
#endif

	/*
	for (i = begin; i != end; ++i)
	{
		++i;
		m.set_pixel(*i, y);
	}
	*/
}

void Simple_aa_rasterizer::apply_mask(Color c)
{
	for (int y = 0; y < m.height(); ++y)
	{
		for (int x = 0; x < m.width(); ++x)
		{
			int v = m.alpha_at(x, y);

			if (v)
			{
				if (v == m.alpha_max())
				{
					rgb->set_pixel(x, y, c);
				}
				else
				{
					Color o = rgb->get_pixel(x, y);

					rgb->set_pixel(x, y, mix(o, c, ((double)v) / 32));
				}

				m.clear_alpha(x, y);
			}
		}
	}
}

void Simple_aa_rasterizer::begin_shape()
{
	Scanline_generator::clear();
}

void Simple_aa_rasterizer::begin_pgon()
{
	Scanline_generator::begin_draw();
}

void Simple_aa_rasterizer::vertex(const Vertex& v)
{
	Scanline_generator::vertex(v);
}

void Simple_aa_rasterizer::end_pgon(bool b)
{
	pixel_on_flag = b;
	Scanline_generator::end_draw();
}

void Simple_aa_rasterizer::end_shape(Color c)
{
	// apply mask to backbuffer
	apply_mask(c);
}

//--------------------------------------------------------------------

Rasterizer* make_bad_rasterizer(Rgb_buffer* b)
{
	return new Simple_aa_rasterizer(b);
}

