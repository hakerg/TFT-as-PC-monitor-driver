#pragma once
#include <ctime>
#include "ArduinoIO.h"
#include "Color16.h"
#include "Region.h"
#include "Array2D.h"

using namespace System::Threading;

ref class RegionPusher
{
public:

	const int maxRegionSizeX;
	const int maxRegionSizeY;
	const int maxRegionPixelCount = maxRegionSizeX * maxRegionSizeY;
	const int blockColumns = ArduinoIO::width / maxRegionSizeX;
	const int blockRows = ArduinoIO::height / maxRegionSizeY;

private:

	int* drawingTimes;
	double* drawingPriorities;
	bool running;
	Thread^ sendingThread;
	int currentBlockX = 0;
	int currentBlockY = 0;
	Array2D<Region*> buffer;
	Array2D<int> screenPriority;

	clock_t lastFillTime;
	clock_t bufferTimeout;
	int bufferTime;
	int regionsToFill;

	void SendingProc();
	int Contrast(const Color16& regionColor, const Color16& arduinoColor);
	void SetArduinoScreen();
	void FillArduinoBuffer();
	void NextBlock();
	Region* FindBestRegionInBuffer();
	uint8_t ReverseBits(uint8_t b);
	void AppendContrastAndPriority(Region& newRegion, Region& oldRegion);
	inline int Pow2(int v);
	inline int Pow4(int v);
	void UpdateArduinoScreen(Region& region);

public:

	ArduinoIO arduino;

	// [y][x]
	Array2D<Color16> arduinoScreen;
	Array2D<Color16> target;

	RegionPusher(int maxRegionSizeX, int maxRegionSizeY, bool enableTouch);
	virtual ~RegionPusher();
	void TryStart(LPCTSTR portName, long baudRate);
	void Stop();
};
