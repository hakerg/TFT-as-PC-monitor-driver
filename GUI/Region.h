#pragma once
#include "Color16.h"

ref class Region {
private:
	static constexpr int drawConstTime_us = 95;
	static constexpr int drawPixelTime_us = 3;

	int GetDrawingTime_us()
	{
		return GetDrawingTime_us(sizeX * sizeY);
	}

	static int log2(int n) {
		int result = 0;
		while (n >>= 1) {
			result++;
		}
		return result;
	}

public:
	static constexpr int bytesSize = 5;

	const int x;
	const int y;
	const int sizeX;
	const int sizeY;
	Color16 color;
	const int endX;
	const int endY;
	const int drawingTime_us;

	Region(int x, int y, int sizeX, int sizeY) :
		x(x), y(y), sizeX(sizeX), sizeY(sizeY), endX(x + sizeX), endY(y + sizeY),
		drawingTime_us(GetDrawingTime_us()) {}

	void FillData(uint8_t* data) {
		data[0] = x & 0xFF;
		data[1] = y;
		data[2] = (x >> 8) | (log2(sizeX) << 1) | (log2(sizeY) << 5);
		*(uint16_t*)&data[3] = color.To565();
	}

	static int GetDrawingTime_us(int pixelCount) {
		return pixelCount * drawPixelTime_us + drawConstTime_us;
	}
};

