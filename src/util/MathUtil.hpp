#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

namespace util
{
	// floor, ceil and round functions from the standard library are extremely slow for some reason, so we're going to
	// implement faster versions of these functions by ourselves.

	static inline long fastFloor(const float value)
	{
		long l = (long)value;
		return l - (l > value);
	}

	static inline long fastRound(const float value)
	{
		return fastFloor(value + 0.5f);
	}

	static inline long fastCeil(const float value)
	{
		long l = (long)value;
		return l + (l > value);
	}
}
