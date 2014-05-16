/*
 * This file contains the implementation of our specialized spaceclock
 * dock app by overloading the render() function in the DockApp class
 *
 */

#include "clockapp.h"
#include "rgb_buffer.h"
#include "raster_bad.h"
#include "config.h"

#include <cstdlib>
#include <cmath>
#include <iostream>

class Vector_add_rotator
{
	double angle;
	Vertex add;

public:
	Vector_add_rotator(const Vertex& v, double a) : add(v), angle(a) { }

	void operator() (Vertex& v) 
	{ 
		Vertex nv;
		nv.x = add.x - sin(angle) * v.y - cos(angle) * v.x;
		nv.y = add.y + cos(angle) * v.y - sin(angle) * v.x;
		v = nv;
	}
};

class TimeData
{
	char h, m, s;
	int usec;

	void init(time_t ut, unsigned usec0 = 0)
	{
		struct tm *lt = localtime(&ut);

		h = lt->tm_hour;
		m = lt->tm_min;
		s = lt->tm_sec;

		usec = usec0;
	}

public:
	TimeData()
	{
		time_t curtime = time(0);

		struct timeval tv;
		struct timezone tz;
		gettimeofday(&tv, &tz);

		init(curtime, tv.tv_usec);
	}

	TimeData(time_t ut, unsigned usec0)
	{
		init(ut, usec0);
	}

	int exact_second() const
	{
		return s;
	}

	double seconds() const
	{
		return s + (double)usec / 1000000;
	}

	double minutes() const
	{
		return m + (double)s / 60;
	}

	double hours() const
	{
		return h + (double)m / 60;
	}
};

class ClockDraw
{
	Rgb_buffer* buf;
	Rasterizer* ras;
	ClockConfig* cfg;

public:
	ClockDraw(ClockConfig* c, Rgb_buffer* b, Rasterizer* r);

	void draw_floating_tick(double angle);
	void draw_simple_hand(double angle, double len);

	void draw_shape(double angle, const ClockConfig::Shape& shape);
	void draw_hand_shapes(double angle, const ClockConfig::Hand& shape);

	void draw_second_hand(const TimeData& td);
	void draw_minute_hand(const TimeData& td);
	void draw_hour_hand(const TimeData& td);

	void draw_hand(const TimeData& td, int idx);
};

ClockDraw::ClockDraw(ClockConfig* c, Rgb_buffer* b, Rasterizer* r) 
{
	cfg = c;
	buf = b;
	ras = r;
}

class Rasterize_vertex
{
	Rasterizer* r;
public:
	Rasterize_vertex(Rasterizer* ras) : r(ras) { }
	inline void operator() (const Vertex& v) { r->vertex(v); }
};

template <class It>
void draw_polygon(Rasterizer* ras, It b, It e, Color c)
{
	ras->begin_shape();
	ras->begin_pgon();

	std::for_each(b, e, Rasterize_vertex(ras));

	ras->end_pgon(true);
	ras->end_shape(c);
}

void ClockDraw::draw_floating_tick(double angle)
{
	Vertex tri[3];
	Vertex* v = tri;

	const double xc = 28.5;
	const double yc = 28.5;

	v->x = xc + sin((angle)*M_PI/180) * 25;
	v->y = yc - cos((angle)*M_PI/180) * 25;
	v++;

	v->x = xc + sin((angle+10)*M_PI/180) * 20;
	v->y = yc - cos((angle+10)*M_PI/180) * 20;
	v++;

	v->x = xc + sin((angle-10)*M_PI/180) * 20;
	v->y = yc - cos((angle-10)*M_PI/180) * 20;

	draw_polygon(ras, tri, tri+3, Color(255, 255, 255));
}

void ClockDraw::draw_simple_hand(double angle, double len)
{
	Vertex v[] = 
	{
		Vertex(-2, 4),
		Vertex(-2, -len - 1),
		Vertex( 2, -len - 1),
		Vertex( 2, 4),

		Vertex(-1, 3),
		Vertex(-1, -len),
		Vertex( 1, -len),
		Vertex( 1, 3)
	};

	std::for_each(v, v+8, Vector_add_rotator(Vertex(30.5, 30.5), angle*M_PI/180));

	draw_polygon(ras, v,   v+4, Color(255, 255, 255));
	draw_polygon(ras, v+4, v+8, Color(255, 128, 128));
}

void ClockDraw::draw_shape(double angle, const ClockConfig::Shape& shape)
{
	typedef std::vector<Vertex>::iterator VIT;
	typedef std::vector<ClockConfig::Polygon>::const_iterator PIT;

	ras->begin_shape();
	for (PIT pp = shape.pgons.begin(); pp != shape.pgons.end(); ++pp)
	{
		std::vector<Vertex> v = pp->v;

		std::for_each(v.begin(), v.end(), Vector_add_rotator(shape.origin, angle));

		ras->begin_pgon();

		std::for_each(v.begin(), v.end(), Rasterize_vertex(ras));

		ras->end_pgon(true);
	}
	ras->end_shape(shape.color);
}

void ClockDraw::draw_hand_shapes(double angle, const ClockConfig::Hand& h)
{
	typedef std::vector<ClockConfig::Shape>::const_iterator SIT;

	// exact option
	if (h.exact)
	{
		angle -= std::fmod(angle, 6.0);
	}

	double a = (angle+180)*M_PI/180;
	for (SIT sp = h.shapes.begin(); sp != h.shapes.end(); ++sp)
	{
		draw_shape(a, *sp);
	}
}

void ClockDraw::draw_second_hand(const TimeData& td)
{
	double angle = td.seconds() * 6;

	draw_hand_shapes(angle, cfg->get_hand(ClockConfig::second_hand));
}

void ClockDraw::draw_minute_hand(const TimeData& td)
{
	double angle = td.minutes() * 6;

	draw_hand_shapes(angle, cfg->get_hand(ClockConfig::minute_hand));
}

void ClockDraw::draw_hour_hand(const TimeData& td)
{
	double angle = td.hours() * 30;

	draw_hand_shapes(angle, cfg->get_hand(ClockConfig::hour_hand));
}

void ClockDraw::draw_hand(const TimeData& td, int which)
{
	switch (which)
	{
		case ClockConfig::second_hand: draw_second_hand(td); break;
		case ClockConfig::minute_hand: draw_minute_hand(td); break;
		case ClockConfig::hour_hand:   draw_hour_hand(td);   break;
	}
}

//--------------------------------------------------------------------

ClockApp::ClockApp(int argc, char** argv, ClockConfig* config, GdkPixbuf* pixbuf) : 
	DockApp(argc, argv, "SpaceClock", pixbuf), 
	rgb(new Rgb_buffer(64, 64)) 
{
	ras = make_bad_rasterizer(rgb);

	cfg = config;

	back = new Rgb_buffer(pixbuf);
	*rgb = *back;
	//rgb->clear();

	cd = new ClockDraw(cfg, rgb, ras);

	last_exact_second = -1;

	if (cfg->get_hand(ClockConfig::second_hand).exact)
	{
		set_sleep_time(10000);
	}
	else
	{
		set_sleep_time(100000);
	}
}

ClockApp::~ClockApp()
{
	delete cd;
}

void ClockApp::render(bool force)
{
	//int x = std::rand() % 56;
	//int y = std::rand() % 56;

	TimeData td; // get current time

	if (!force && 
		cfg->get_hand(ClockConfig::second_hand).exact &&
		td.exact_second() == last_exact_second)
	{
		return;
	}

	last_exact_second = td.exact_second();

	*rgb = *back;

	for (int i = 0; i < 3; ++i)
	{
		int h = cfg->get_hand_index(i);
		cd->draw_hand(td, h);
	}

	draw(0, 0, rgb);
}

