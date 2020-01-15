#include "RegionPusher.h"
#include <algorithm>
#include <ctime>

void RegionPusher::SendingProc()
{
	while (running)
	{
		int x = currentBlockX * maxRegionSizeX + (rand() & (maxRegionSizeX - 1));
		int y = currentBlockY * maxRegionSizeY + (rand() & (maxRegionSizeY - 1));

		if (arduinoScreen.At(x, y) != target.At(x, y))
		{
			Region* region = new Region(x, y, 1, 1, target.At(x, y));
			region->contrast = Contrast(arduinoScreen.At(x, y), region->color) * screenPriority.At(x, y);
			region->priority = region->contrast * drawingPriorities[1];

			int newSizeX = 2;
			while (newSizeX <= maxRegionSizeX)
			{
				Region* newRegion = new Region(region->x / newSizeX * newSizeX, region->y,
					newSizeX, 1, region->color);

				AppendContrastAndPriority(*newRegion, *region);

				if (newRegion->priority > region->priority)
				{
					delete region;
					region = newRegion;
				}
				else
				{
					delete newRegion;
					break;
				}

				newSizeX *= 2;
			}

			int newSizeY = 2;
			while (newSizeY <= maxRegionSizeY)
			{
				Region* newRegion = new Region(region->x, region->y / newSizeY * newSizeY,
					region->sizeX, newSizeY, region->color);

				AppendContrastAndPriority(*newRegion, *region);

				if (newRegion->priority > region->priority)
				{
					delete region;
					region = newRegion;
				}
				else
				{
					delete newRegion;
					break;
				}

				newSizeY *= 2;
			}

			if (region->priority > buffer.At(currentBlockX, currentBlockY)->priority)
			{
				std::swap(buffer.At(currentBlockX, currentBlockY), region);
			}

			delete region;
		}

		NextBlock();
	}
}

int RegionPusher::Contrast(const Color16& regionColor, const Color16& arduinoColor)
{
	return Pow2(regionColor.r - arduinoColor.r) * 2
		+ Pow2(regionColor.g - arduinoColor.g)
		+ Pow2(regionColor.b - arduinoColor.b) * 2;
}

void RegionPusher::SetArduinoScreen()
{
	Color16 fill;
	fill.r = 0;
	fill.g = 0;
	fill.b = 31;
	for (int y = 0; y < ArduinoIO::height; y++)
	{
		for (int x = 0; x < ArduinoIO::width; x++)
		{
			arduinoScreen.At(x, y) = fill;
		}
	}
}

void RegionPusher::FillArduinoBuffer()
{
	clock_t currentTime = clock();
	int maxRegions;
	if (bufferTimeout <= currentTime)
	{
		maxRegions = ArduinoIO::maxRegionCount;
	}
	else
	{
		maxRegions = (currentTime - lastFillTime) * regionsToFill / (bufferTimeout - lastFillTime) + 1;
	}
	lastFillTime = currentTime;
	while (arduino.AvailableSpace() >= ArduinoIO::regionSendSize && maxRegions && running)
	{
		Region* bestRegion = FindBestRegionInBuffer();
		if (bestRegion)
		{
			arduino.Push(*bestRegion);
			UpdateArduinoScreen(*bestRegion);
			bestRegion->priority = 0.0;
			regionsToFill--;
			bufferTime += drawingTimes[bestRegion->sizeX * bestRegion->sizeY];
			if (!regionsToFill)
			{
				bufferTimeout = clock() + bufferTime / 1000;
				bufferTime = 0;
				regionsToFill = ArduinoIO::maxRegionCount;
			}
		}
		else
		{
			break;
		}
		maxRegions--;
	}
}

void RegionPusher::NextBlock()
{
	currentBlockX++;
	if (currentBlockX == blockColumns)
	{
		currentBlockX = 0;
		currentBlockY++;
		if (currentBlockY == blockRows)
		{
			currentBlockY = 0;
			FillArduinoBuffer();
		}
	}
}

Region* RegionPusher::FindBestRegionInBuffer()
{
	Region* bestRegion = nullptr;
	for (int y = 0; y < blockRows; y++)
	{
		for (int x = 0; x < blockColumns; x++)
		{
			Region* region = buffer.At(x, y);
			if (region->priority > 0.0)
			{
				if (!bestRegion)
				{
					bestRegion = region;
				}
				else if (region->priority > bestRegion->priority)
				{
					bestRegion = region;
				}
			}
		}
	}
	return bestRegion;
}

uint8_t RegionPusher::ReverseBits(uint8_t b)
{
	b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
	b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
	b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
	return b;
}

void RegionPusher::AppendContrastAndPriority(Region& newRegion, Region& oldRegion)
{
	newRegion.contrast = oldRegion.contrast;

	int beginX = newRegion.x;
	int beginY = newRegion.y;
	int endX = newRegion.endX;
	int endY = newRegion.endY;

	if (newRegion.x < oldRegion.x)
	{
		endX = oldRegion.x;
	}
	else if (newRegion.endX > oldRegion.endX)
	{
		beginX = oldRegion.endX;
	}
	else if (newRegion.y < oldRegion.y)
	{
		endY = oldRegion.y;
	}
	else
	{
		beginY = oldRegion.endY;
	}

	for (int y = beginY; y < endY; y++)
	{
		for (int x = beginX; x < endX; x++)
		{
			newRegion.contrast += (Contrast(arduinoScreen.At(x, y), target.At(x, y))
				- Contrast(newRegion.color, target.At(x, y))) * screenPriority.At(x, y);
		}
	}

	newRegion.priority = newRegion.contrast * drawingPriorities[newRegion.sizeX * newRegion.sizeY];
}

inline int RegionPusher::Pow2(int v)
{
	return v * v;
}

inline int RegionPusher::Pow4(int v)
{
	return v * v * v * v;
}

void RegionPusher::UpdateArduinoScreen(Region& region)
{
	for (int y = region.y; y < region.endY; y++)
	{
		for (int x = region.x; x < region.endX; x++)
		{
			arduinoScreen.At(x, y) = region.color;
		}
	}
}

RegionPusher::RegionPusher(int maxRegionSizeX, int maxRegionSizeY, bool enableTouch) :
	maxRegionSizeX(maxRegionSizeX),
	maxRegionSizeY(maxRegionSizeY),
	buffer(blockColumns, blockRows),
	screenPriority(ArduinoIO::width, ArduinoIO::height),
	arduinoScreen(ArduinoIO::width, ArduinoIO::height),
	target(ArduinoIO::width, ArduinoIO::height),
	arduino(enableTouch)
{
	drawingTimes = new int[maxRegionPixelCount + 1];
	drawingPriorities = new double[maxRegionPixelCount + 1];

	int maxDistance = (int)sqrt(Pow2(ArduinoIO::width / 2 + 1) + Pow2(ArduinoIO::height / 2 + 1)) + 1;
	for (int y = 0; y < ArduinoIO::height; y++)
	{
		for (int x = 0; x < ArduinoIO::width; x++)
		{
			screenPriority.At(x, y) = maxDistance -
				(int)sqrt(Pow2(x - ArduinoIO::width / 2) + Pow2(y - ArduinoIO::height / 2));
		}
	}
	for (int pc = 1; pc <= maxRegionPixelCount; pc *= 2)
	{
		drawingTimes[pc] = arduino.GetDrawingTime(pc);
		drawingPriorities[pc] = 1.0 / drawingTimes[pc];
	}
	for (int y = 0; y < blockRows; y++)
	{
		for (int x = 0; x < blockColumns; x++)
		{
			buffer.At(x, y) = new Region(0, 0, 1, 1, Color16());
		}
	}
}

RegionPusher::~RegionPusher()
{
	if (running)
	{
		Stop();
	}
	for (int y = 0; y < blockRows; y++)
	{
		for (int x = 0; x < blockColumns; x++)
		{
			delete buffer.At(x, y);
		}
	}
	delete[] drawingTimes;
	delete[] drawingPriorities;
}

void RegionPusher::TryStart(LPCTSTR portName, long baudRate)
{
	lastFillTime = 0;
	bufferTimeout = 0;
	regionsToFill = ArduinoIO::maxRegionCount;
	bufferTime = 0;
	SetArduinoScreen();
	arduino.TryStart(portName, baudRate);
	running = arduino.serialPort->IsConnected();
	sendingThread = gcnew Thread(gcnew ThreadStart(this, &RegionPusher::SendingProc));
	sendingThread->Start();
}

void RegionPusher::Stop()
{
	running = false;
	sendingThread->Join();
	arduino.Stop();
}