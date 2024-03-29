#pragma once
#include <Windows.h>
#include <cstdint>

using namespace System::Collections::Generic;

ref class Color16
{
private:
	static initonly array<int, 2>^ ditherR = gcnew array<int, 2>(2, 4) { { 7, 1, 6, 0 }, { 3, 5, 2, 4 } };
	static initonly array<int, 2>^ ditherG = gcnew array<int, 2>(2, 2) { { 1, 3 }, { 2, 0 } };
	static initonly array<int, 2>^ ditherB = gcnew array<int, 2>(2, 4) { { 4, 2, 5, 3 }, { 0, 6, 1, 7 } };

public:
	int r;
	int g;
	int b;

	Color16(int r, int g, int b) : r(r), g(g), b(b) {}

	Color16(const RGBQUAD& color)
	{
		r = color.rgbRed >> 3;
		g = color.rgbGreen >> 2;
		b = color.rgbBlue >> 3;
	}

	Color16(const RGBQUAD& color, int x, int y) : Color16(color)
	{
		if ((color.rgbRed & 7) > ditherR[y & 1, x & 3] && r < 0x1F)
		{
			r++;
		}
		if ((color.rgbGreen & 3) > ditherG[y & 1, x & 1] && g < 0x3F)
		{
			g++;
		}
		if ((color.rgbBlue & 7) > ditherB[y & 1, x & 3] && b < 0x1F)
		{
			b++;
		}
	}

	uint16_t To565()
	{
		return (r << 11) | (g << 5) | b;
	}
};

