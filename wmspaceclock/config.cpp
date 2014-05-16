
#include "config.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>

#include <sys/stat.h>
#include <unistd.h>

#include "spaceclock.h"

using std::string;
using std::ifstream;
using std::istringstream;

class MyClockConfig : public ClockConfig
{
	Hand hands[3];
	int layers[3];
	string back_image_name;
	std::map<string, Color> named_colors;

	void read_vertices(istringstream& is, Polygon& pgon);
	void read_pgon(istringstream& is, Polygon& pgon);
	void read_shape(istringstream& is, Shape& s);
	void read_hand(istringstream& is, Hand& h);

public:
	MyClockConfig();

	virtual ClockConfig::hand_idx get_hand_index(int) const;
	virtual const Hand& get_hand(hand_idx) const;
	virtual string bg_image_name() const;

	virtual void parse(const char*);
	virtual bool load(const char*);
};

ClockConfig::hand_idx MyClockConfig::get_hand_index(int i) const
{
	return (hand_idx)layers[i];
}

const ClockConfig::Hand& MyClockConfig::get_hand(hand_idx i) const
{
	return hands[i];
}

string MyClockConfig::bg_image_name() const 
{
	return back_image_name;
}

ClockConfig* make_config()
{
	return new MyClockConfig;
}

//--------------------------------------------------------------------

void flag_value(string flag, bool& b)
{
	if (flag == "true"  || flag == "on"  || flag == "1") b = true;
	if (flag == "false" || flag == "off" || flag == "0") b = false;
}

//--------------------------------------------------------------------

MyClockConfig::MyClockConfig()
{
	for (int i = 0; i < 3; ++i)
	{
		layers[i] = i;
	}
}

void MyClockConfig::read_vertices(istringstream& is, Polygon& pgon)
{
	while (!is.eof())
	{
		string token;
		is >> token;
		if (token.empty()) continue;

		if (token == "v")
		{
			double x, y;
			is >> x >> y;
			
			pgon.v.push_back(Vertex(x, y));
		}
		else if (token == "end")
		{
			return;
		}
		else throw error(token);
	}
}

void MyClockConfig::read_pgon(istringstream& is, Polygon& pgon)
{
	while (is)
	{
		string token;
		is >> token;
		if (token.empty()) continue;

		if (token == "fill")
		{
			string flag;
			is >> flag;

			flag_value(flag, pgon.on);
		}
		else if (token == "begin")
		{
			read_vertices(is, pgon);
		}
		else if (token == "end")
		{
			return;
		}
		else throw error(token);
	}
}

void MyClockConfig::read_shape(istringstream& is, Shape& shape)
{
	while (is)
	{
		string token;
		is >> token;
		if (token.empty()) continue;

		if (token == "color")
		{
			int r, g, b;
			is >> r >> g >> b;
			shape.color = Color(r, g, b);
		}
		else if (token == "preset_color" || token == "named_color")
		{
			string name;
			is >> name;
			shape.color = named_colors[name];
		}
		else if (token == "origin")
		{
			double x, y;
			is >> x >> y;
			shape.origin = Vertex(x, y);
		}
		else if (token == "pgon")
		{
			Polygon p;
			read_pgon(is, p);
			shape.pgons.push_back(p);
		}
		else if (token == "end")
		{
			return;
		}
		else throw error(token);
	}

}

void MyClockConfig::read_hand(istringstream& is, Hand& h)
{
	while (is)
	{
		string token;
		is >> token;
		if (token.empty()) continue;

		if (token == "exact")
		{
			string flag;
			is >> flag;
			flag_value(flag, h.exact);
		}
		else if (token == "shape")
		{
			Shape shape;
			read_shape(is, shape);
			h.shapes.push_back(shape);
		}
		else if (token == "end")
		{
			return;
		}
		else throw error(token);
	}
}

bool MyClockConfig::load(const char* filename)
{
	string cfg;

	ifstream is(full_filename(filename).c_str());
	if (!is) return false;

	while (is)
	{
		string line;
		getline(is, line);
		if (line[0] != '#') cfg += line + '\n';
	}

	parse(cfg.c_str());

	return true;
}

void MyClockConfig::parse(const char* cfg)
{
	istringstream is(cfg);

	while (is)
	{
		string token;

		is >> token;
		if (token.empty()) continue;

		if (token == "theme" || token == "import")
		{
			string filename;
			is >> filename;
			try
			{
				load(filename.c_str());
			}
			catch (error& e)
			{
				std::cerr << "error reading file " << filename 
					<< " at token: " << e.token << std::endl;
			}
		}
		else if (token == "hand_order")
		{
			for (int i = 0; i < 3; ++i)
			{
				string hand;

				is >> hand;

				int which = 0;
				if (hand == "second") which = second_hand;
				else if (hand == "minute") which = minute_hand;
				else if (hand == "hour")   which = hour_hand;
				else throw error("hand_order");

				layers[which] = i;
			}
		}
		else if (token == "bg_image")
		{
			is >> back_image_name;
		}
		else if (token == "def_color")
		{
			string name;
			is >> name;

			int r, g, b;
			is >> r >> g >> b;

			named_colors[name] = Color(r, g, b);
		}
		else if (token == "second_hand")
		{
			read_hand(is, hands[second_hand]);
		}
		else if (token == "minute_hand")
		{
			read_hand(is, hands[minute_hand]);
		}
		else if (token == "hour_hand")
		{
			read_hand(is, hands[hour_hand]);
		}
		else throw error(token);
	}
}

