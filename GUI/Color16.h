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
	int r = 0;
	int g = 0;
	int b = 0;

	Color16() {}

	Color16(Color16^ color)
	{
		Set(color);
	}

	Color16(const RGBQUAD& color)
	{
		Set(color);
	}

	Color16(const RGBQUAD& color, int x, int y)
	{
		SetDithered(color, x, y);
	}

	void Set(Color16^ color)
	{
		r = color->r;
		g = color->g;
		b = color->b;
	}

	void Set(const RGBQUAD& color)
	{
		r = color.rgbRed >> 3;
		g = color.rgbGreen >> 2;
		b = color.rgbBlue >> 3;
	}

	void SetDithered(const RGBQUAD& color, int x, int y)
	{
		Set(color);
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

