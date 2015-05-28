#ifndef _SPACE_CONFIG_H_
#define _SPACE_CONFIG_H_

#include <vector>
#include <string>

#include "rgb_buffer.h"
#include "scanliner.h"

class ClockConfig
{
public:
	class error
	{
	public:
		std::string token;
		error(std::string s) : token(s) { }
	};

	struct Polygon
	{
		std::vector<Vertex> v;
		bool on;
	};

	struct Shape
	{
		Vertex origin;
		std::vector<Polygon> pgons;
		Color color;
	};

	struct Hand
	{
		bool exact;
		std::vector<Shape> shapes;
	};

	enum hand_idx { second_hand, minute_hand, hour_hand };

	virtual hand_idx get_hand_index(int) const = 0;
	virtual const Hand& get_hand(hand_idx) const = 0;
	virtual std::string bg_image_name() const = 0;

	virtual void parse(const char*) = 0;
	virtual bool load(const char*) = 0;

	virtual ~ClockConfig() { }
};

ClockConfig* make_config();

#endif // _SPACE_CONFIG_H_
