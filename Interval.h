#pragma once

#include "Utilities.h";

class Interval 
{
public:
	double min, max;

	Interval() : min(+INF), max(-INF) {}

	Interval(float _min, float _max) : min(_min), max(_max) {}

	bool contains(float x) const
	{
		return min <= x && x <= max;
	}

	bool surrounds(float x) const
	{
		return min < x && x < max;
	}

	float clamp(float x) const
	{
		if (x < min) return min;
		if (x > max) return max;
		return x;
	}

	static const Interval empty, universe;

};

const static Interval empty(+INF, -INF);
const static Interval universe(-INF, +INF);