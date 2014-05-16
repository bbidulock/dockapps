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

#include "config.h"
#include "clockapp.h"

#include "master.xpm"

#include <gdk-pixbuf/gdk-pixbuf.h>

#include <sys/stat.h>
#include <unistd.h>

#include <string>
#include <iostream>

using std::string;

string version(PRGVER);

bool file_exists(const char* filename)
{
	struct stat s;
	int r = stat(filename, &s);

	if (r) return false;

	return true;
}

string full_filename(string name)
{
	if (name[0] == '/')
	{
		return name;
	}

	const char* home = getenv("HOME");
	if (name[0] == '~' && home)
	{
		return home + name.substr(1);
	}

	if (home)
	{
		string fn = string(home) + "/.spaceclock/" + name;
		if (file_exists(fn.c_str())) return fn;
	}

	string f1 = INSTALL_PREFIX "/share/spaceclock/" + name;
	if (file_exists(f1.c_str())) return f1;

	return name;
}

//--------------------------------------------------------------------

static const char* default_config =
	"bg_image gfx/2.png "
	"second_hand "
	" exact false "
	" shape "
	"  origin 32 32 "
	"  color 255 255 255 "
	"  pgon "
	"   fill 1 "
	"   begin "
	"    v -3 20 "
	"    v  0 25 "
	"    v  3 20 "
	"   end "
	"  end "
	" end "
	"end "
	"minute_hand "
	" exact false "
	" shape "
	"  origin 32 32 "
	"  color 255 255 255 "
	"  pgon "
	"   fill 1 "
	"   begin "
	"    v -2.0 -3.0 "
	"    v -0.5 20.0 "
	"    v  0.5 20.0 "
	"    v  2.0 -3.0 "
	"   end "
	"  end "
	" end "
	"end "
	"hour_hand "
	" exact false "
	" shape "
	"  origin 32 32 "
	"  color 255 255 255 "
	"  pgon "
	"   fill 1 "
	"   begin "
	"    v -2.5 -3.0 "
	"    v -0.5 12.0 "
	"    v  0.5 12.0 "
	"    v  2.5 -3.0 "
	"   end "
	"  end "
	" end "
	"end";

int main(int argc, char* argv[])
{
	if (!gdk_init_check(&argc, &argv)) 
	{
		std::cerr << "GDK initialization failed, check $DISPLAY environment variable." << std::endl;
		return 1;
	}

	gdk_rgb_init();

	if (argc > 1)
	{
		string param(argv[1]);

		if (param == "-v" || param == "-V" || param == "--version")
		{
			std::cout << "Spaceclock " << version << " compiled on " << __DATE__ << std::endl;
			std::cout << "Copyright 2001-2002 Attila Tajti" << std::endl;
			std::cout << "Install prefix: " << INSTALL_PREFIX << std::endl;

			return 0;
		}
		if (param == "-h" || param == "--help")
		{
			std::cout << "Usage:" << std::endl;
			std::cout << "  spaceclock [theme]" << std::endl << std::endl;
			std::cout << "Example:" << std::endl;
			std::cout << "  spaceclock spacehawks_square.theme &" << std::endl << std::endl;
			std::cout << "Have a look at the data directory (" << INSTALL_PREFIX << 
			"/share/spaceclock)" << std::endl << "to find more theme files." << std::endl;

			return 0;
		}
	}

	ClockConfig* cfg = make_config();
	const char* cfgname = argc > 1 ? argv[1] : 0;
	if (cfgname)
	{
		cfg->load(cfgname);
	}
	else
	{
		cfg->parse(default_config);
	}

	string bg_name = full_filename(cfg->bg_image_name());
	GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file(bg_name.c_str());

	if (pixbuf == 0) 
	{
		pixbuf = gdk_pixbuf_new_from_xpm_data(const_cast<const char**>(master_xpm));
		assert(pixbuf);
	}

	ClockApp clockapp(argc, argv, cfg, pixbuf);
	clockapp.run();

	return 0;
}

