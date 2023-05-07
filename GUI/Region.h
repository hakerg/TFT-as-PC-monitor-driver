#pragma once
#include "Color16.h"

ref class Region {
private:
	static constexpr int drawConstTime_us = 103;
	static constexpr int drawPixelTime_us = 3;

	int GetDrawingTime_us()
	{
		return size * size * drawPixelTime_us + drawConstTime_us;
	}

public:
	static constexpr int bytesSize = 5;

	const int x;
	const int y;
	const int size;
	Color16 color;
	const int endX;
	const int endY;
	const int drawingTime_us;

	Region(int x, int y) :
		x(x), y(y), size(1), endX(x + size), endY(y + size),
		drawingTime_us(GetDrawingTime_us()) {}

	Region(int x, int y, int size) :
		x(x), y(y), size(size), endX(x + size), endY(y + size),
		drawingTime_us(GetDrawingTime_us()) {}
};

