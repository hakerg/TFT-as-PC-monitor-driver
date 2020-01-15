#pragma once
#include <Windows.h>
#include <cstdint>

class Color16
{
private:

	static const int ditherR[2][4];
	static const int ditherG[2][2];
	static const int ditherB[2][4];

public:

	int r = 0;
	int g = 0;
	int b = 0;

	Color16();
	Color16(const RGBQUAD& color);
	Color16(const RGBQUAD& color, int x, int y);

	void Set(const RGBQUAD& color);
	void SetDithered(const RGBQUAD& color, int x, int y);

	uint16_t To565();

	bool operator==(const Color16& rhs);
	bool operator!=(const Color16& rhs);

};

