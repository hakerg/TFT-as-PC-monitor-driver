#include "Color16.h"

const int Color16::ditherR[2][4] = { 7, 1, 6, 0, 3, 5, 2, 4 };
const int Color16::ditherG[2][2] = { 1, 3, 2, 0 };
const int Color16::ditherB[2][4] = { 4, 2, 5, 3, 0, 6, 1, 7 };

Color16::Color16()
{
}

Color16::Color16(const RGBQUAD& color)
{
	Set(color);
}

Color16::Color16(const RGBQUAD& color, int x, int y)
{
	SetDithered(color, x, y);
}

void Color16::Set(const RGBQUAD& color)
{
	r = color.rgbRed >> 3;
	g = color.rgbGreen >> 2;
	b = color.rgbBlue >> 3;
}

void Color16::SetDithered(const RGBQUAD& color, int x, int y)
{
	Set(color);
	if ((color.rgbRed & 7) > ditherR[y & 1][x & 3] && r < 0x1F)
	{
		r++;
	}
	if ((color.rgbGreen & 3) > ditherG[y & 1][x & 1] && g < 0x3F)
	{
		g++;
	}
	if ((color.rgbBlue & 7) > ditherB[y & 1][x & 3] && b < 0x1F)
	{
		b++;
	}
}

uint16_t Color16::To565()
{
	return (r << 11) | (g << 5) | b;
}

bool Color16::operator==(const Color16& rhs)
{
	return r == rhs.r && g == rhs.g && b == rhs.b;
}

bool Color16::operator!=(const Color16& rhs)
{
	return r != rhs.r || g != rhs.g || b != rhs.b;
}
