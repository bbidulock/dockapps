#ifndef _SPACE_SCANLINER_H_
#define _SPACE_SCANLINER_H_

#include <vector>

class Vertex
{
public:
	double x, y;

	Vertex() { }
	Vertex(double x0, double y0) { x = x0; y = y0; }
};

//--------------------------------------------------------------------

class Scanline_generator
{
protected:
	typedef std::vector<int>::iterator xit;
	void dump_scanlines();

	void begin_draw();
	void vertex(const Vertex&);
	void end_draw();

	void clear();

	struct bounding_box
	{
		int xmin;
		int ymin;
		int xmax;
		int ymax;

		void clear(int w, int h)
		{
			xmin = w;
			xmax = 0;
			ymin = h;
			ymax = 0;
		}
		void add(int x, int y)
		{
			if (x <  xmin) xmin = x;
			if (x >= xmax) xmax = x + 1;
			if (y <  ymin) ymin = y;
			if (y >= ymax) ymax = y + 1;
		}
	};

private:
	int xdiv, ydiv;

	int xsize, ysize;

	bounding_box bbox;

	std::vector< std::vector<int> > scanlines;

	struct pos
	{
		int x;
		int y;

		pos() { }
		pos(int x0, int y0) : x(x0), y(y0) { }
	};

	void mark_pixel(int, int);
	void mark_line(const pos&, const pos&);
	void rasterize_scanlines();
	pos vertex_pos(const Vertex&);

	// vertex processor data
	int vertex_no;

	pos first_pos;
	int first_dy;

	pos prev_pos;
	int prev_dy;

public:
	Scanline_generator(int, int, int, int);
	virtual ~Scanline_generator() { }

	virtual void rasterize_scanline(int, xit, xit) = 0;
};

#endif // _SPACE_SCANLINER_H_
