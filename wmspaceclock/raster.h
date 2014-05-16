#ifndef _SPACE_RASTER_H_
#define _SPACE_RASTER_H_

#include "rgb_buffer.h"
#include "scanliner.h"

class Rasterizer
{
public:
	virtual void begin_shape() = 0;
	virtual void begin_pgon() = 0;
	virtual void vertex(const Vertex&) = 0;
	virtual void end_pgon(bool) = 0;
	virtual void end_shape(Color c) = 0;
};

#endif // _SPACE_RASTER_H_
