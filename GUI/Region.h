#pragma once
#include "Color16.h"

class Region
{
public:

	const int x;
	const int y;
	const int sizeX;
	const int sizeY;
	Color16 color;
	const int endX;
	const int endY;
	int contrast = 0;
	double priority = 0.0;

	Region(int x, int y, int sizeX, int sizeY, const Color16& color);
	virtual ~Region();

	bool operator>(const Region& rhs);

};

