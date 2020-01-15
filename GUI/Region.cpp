#include "Region.h"

Region::Region(int x, int y, int sizeX, int sizeY, const Color16& color) :
	x(x), y(y), sizeX(sizeX), sizeY(sizeY), color(color),
	endX(x + sizeX), endY(y + sizeY)
{
}

Region::~Region()
{
}

bool Region::operator>(const Region& rhs)
{
	return priority > rhs.priority;
}
