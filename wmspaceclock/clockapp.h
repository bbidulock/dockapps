#ifndef _SPACE_CLOCKAPP_H_
#define _SPACE_CLOCKAPP_H_

#include "dockapp.h"

#include <gdk/gdk.h>
#include <sys/time.h>

class Rgb_buffer;
class Rasterizer;
class ClockDraw;
class ClockConfig;

class ClockApp : public DockApp
{
	ClockConfig* cfg;

	Rgb_buffer* rgb;
	Rgb_buffer* back;
	Rasterizer* ras;
	ClockDraw* cd;

	char last_exact_second;

public:
	ClockApp(int, char**, ClockConfig*, GdkPixbuf*);
	virtual ~ClockApp();
	virtual void render(bool);
};

#endif // _SPACE_CLOCKAPP_H_
