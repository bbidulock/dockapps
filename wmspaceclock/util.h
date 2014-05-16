#ifndef _SPACE_UTIL_H_
#define _SPACE_UTIL_H_

#include <cmath>

template <class It>
inline It succ(It i)
{
	It r = i;
	++r;
	return r;
}

template <class It>
inline It pred(It i)
{
	It r = i;
	--r;
	return r;
}

inline int floor_int(double f)
{
	return static_cast<int>(std::floor(f));
}

inline int ceil_int(double f)
{
	return static_cast<int>(std::ceil(f));
}

#endif // _SPACE_UTIL_H_
