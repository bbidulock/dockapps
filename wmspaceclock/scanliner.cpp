
#include "scanliner.h"
#include "util.h"

#include <iostream>
#include <iomanip>
#include <iterator>
#include <algorithm>

//#define DBG

using std::vector;
using std::cout;

Scanline_generator::pos Scanline_generator::vertex_pos(const Vertex& v)
{
	return pos((int)(v.x * xdiv), (int)(v.y * ydiv));
}

Scanline_generator::Scanline_generator(int w, int h, int xd0, int yd0)
{
	xdiv = xd0;
	ydiv = yd0;

	xsize = w * ydiv;
	ysize = h * xdiv;

	scanlines.resize(ysize);
}

void Scanline_generator::clear()
{
	std::fill(scanlines.begin(), scanlines.end(), vector<int>());

	bbox.clear(xsize, ysize);
}

void Scanline_generator::mark_pixel(int x, int y)
{
	scanlines[y].push_back(x);

#ifdef DBG
	cout << "   mark_pixel(" << x << ", " << y << ")" << std::endl;
#endif
}

void Scanline_generator::mark_line(const pos& a, const pos& b)
{
#ifdef DBG
	cout << "  mark_line(" << a.x << ", " << a.y << "; "
		<< b.x << ", " << b.y << ")" << std::endl;
#endif

	if (a.y != b.y)
	{
		int sy = (b.y - a.y > 0) ? 1 : -1;
		for (int y = a.y; y != b.y; y += sy)
		{
			int x = a.x + ((y - a.y) * (a.x - b.x)) / (a.y - b.y);
			mark_pixel(x, y);
		}
	}
	else
	{
		mark_pixel(a.x, a.y);
	}
}

void Scanline_generator::dump_scanlines()
{
	for (int y = bbox.ymin; y < bbox.ymax; ++y)
	{
		vector<int>& sl = scanlines[y];

		cout << std::setw(2) << y << " (" << sl.size() << "): ";

		std::copy(sl.begin() + bbox.xmin, sl.end() + bbox.xmax,
				std::ostream_iterator<int>(cout, " "));

		cout << std::endl;
	}
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

void Scanline_generator::rasterize_scanlines()
{
	for (int y = bbox.ymin; y < bbox.ymax; ++y)
	{
		vector<int>& sl = scanlines[y];
		std::sort(sl.begin(), sl.end());
	}

	for (int y = bbox.ymin; y < bbox.ymax; ++y)
	{
		vector<int>& sl = scanlines[y];
		if (sl.size() > 1)
		{
			std::sort(sl.begin(), sl.end());

			typedef vector<int>::iterator It;

			rasterize_scanline(y, sl.begin(), sl.end());
		}
	}
}

void Scanline_generator::begin_draw()
{
	vertex_no = 0;
}

void Scanline_generator::vertex(const Vertex& v)
{
	pos cur_pos = vertex_pos(v);

	bbox.add(cur_pos.x, cur_pos.y);

	if (vertex_no == 0)
	{
		first_pos = cur_pos;
	}
	else
	{
		int cur_dy = cur_pos.y - prev_pos.y;

		mark_line(prev_pos, cur_pos);

		if (vertex_no == 1)
		{
			first_dy = cur_dy;
		}
		else
		{
			if ((cur_dy * prev_dy) < 0)
			{
				mark_pixel(prev_pos.x, prev_pos.y);
			}
		}

		prev_dy = cur_dy;
	}

	prev_pos = cur_pos;
	++vertex_no;
}

void Scanline_generator::end_draw()
{
	// process last line to close polygon
	mark_line(prev_pos, first_pos);

	int cur_dy = first_pos.y - prev_pos.y;
	if ((cur_dy * prev_dy) < 0)
	{
		mark_pixel(prev_pos.x, prev_pos.y);
	}

	// check sentinel pixel at start of polygon
	if ((cur_dy * first_dy) < 0)
	{
		mark_pixel(first_pos.x, first_pos.y);
	}

	// draw actual segments
	rasterize_scanlines();
}

