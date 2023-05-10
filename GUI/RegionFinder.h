#pragma once
#include "ArduinoBuffer.h"
#include "ArduinoScreen.h"
#include "Color16.h"
#include "Region.h"
#include "ScoredRegion.h"
#include "ThreadSafeQueue.h"
#include "ThreadWrapper.h"
#include <algorithm>
#include <ctime>
#include <vector>

using namespace System;
using namespace System::Threading;
using namespace System::Collections::Generic;

ref class ScoredRegionComparer : public IComparer<ScoredRegion^>
{
public:
	virtual int Compare(ScoredRegion^ a, ScoredRegion^ b)
	{
		if (a->calc.priority != b->calc.priority)
		{
			return a->calc.priority > b->calc.priority ? 1 : -1;
		}

		if (a->sizeY != b->sizeY)
		{
			return a->sizeY > b->sizeY ? -1 : 1;
		}

		if (a->sizeX != b->sizeX)
		{
			return a->sizeX > b->sizeX ? -1 : 1;
		}

		if (a->y != b->y)
		{
			return a->y > b->y ? -1 : 1;
		}

		if (a->x != b->x)
		{
			return a->x > b->x ? -1 : 1;
		}

		return 0;
	}
};

ref class RegionFinder : public ThreadWrapper {
private:
	static const int minBufferTime_us = 100 * 1000; // 100ms

	array<ScoredRegion^, 2>^ initialRegions;
	array<ScoredRegion^, 2>^ regionArray;
	SortedSet<ScoredRegion^> currentRegions;

	void UpdateArduinoScreen(Region^ region)
	{
		for (int y = region->y; y < region->endY; y++)
		{
			for (int x = region->x; x < region->endX; x++)
			{
				arduinoScreen.At(x, y) = region->color;
			}
		}
	}

	void PrepareUnitRegions(ArduinoScreen^ target) {
		for (int x = 0; x < ArduinoScreen::width; x++) {
			for (int y = 0; y < ArduinoScreen::height; y++) {
				regionArray[x, y] = initialRegions[x, y];
				regionArray[x, y]->InitPriority(target->At(x, y), arduinoScreen.At(x, y));
			}
		}
	}

	void TryMerge(int x1, int y1, int x2, int y2, int sizeMultX, int sizeMultY) {
		ScoredRegion^ a = regionArray[x1, y1];
		ScoredRegion^ b = regionArray[x2, y2];
		if (a->calc.priority <= 0 || b->calc.priority <= 0 || a->sizeX != b->sizeX || a->sizeY != b->sizeY) {
			return;
		}
		ScoredRegionCalculation calc(a, b);
		if (calc.priority >= a->calc.priority && calc.priority >= b->calc.priority) {
			regionArray[x1, y1] = gcnew ScoredRegion(x1, y1, a->sizeX * sizeMultX, a->sizeY * sizeMultY, calc);
			a->calc.priority = 0;
			b->calc.priority = 0;
		}
	}

	void MergeRegions() {
		for (int sizeX = 1, sizeY = 1; sizeY <= ArduinoScreen::height;) {
			for (int y = 0; y < ArduinoScreen::height; y++) {
				for (int x = 0; x + sizeX < ArduinoScreen::width; x += sizeX * 2) {
					TryMerge(x, y, x + sizeX, y, 2, 1);
				}
			}
			sizeX *= 2;

			for (int x = 0; x < ArduinoScreen::width; x++) {
				for (int y = 0; y + sizeY < ArduinoScreen::height; y += sizeY * 2) {
					TryMerge(x, y, x, y + sizeY, 1, 2);
				}
			}
			sizeY *= 2;
		}
	}

	void PrepareCurrentRegions() {
		currentRegions.Clear();
		for (int x = 0; x < ArduinoScreen::width; x++) {
			for (int y = 0; y < ArduinoScreen::height; y++) {
				ScoredRegion^ region = regionArray[x, y];
				if (region->calc.priority <= 0) {
					continue;
				}
				currentRegions.Add(region);
			}
		}
	}

	void ProcessFrame(ArduinoScreen^ target) {
		// TODO: add pixel-by-pixel mode in regions
		// TODO: priority depends on last update time
		PrepareUnitRegions(target);
		MergeRegions();
		PrepareCurrentRegions();
	}

	void PushToBuffer(Region^ region) {
		UpdateArduinoScreen(region);
		buffer.Push(region);
	}

	void PushRegionsIfNeeded() {
		//std::cout << "Before: " << buffer.BufferTime_us() << "us, " << buffer.Size() << " in buffer, " << currentRegions.Count << " current\n";
		while (buffer.BufferTime_us() < minBufferTime_us && currentRegions.Count > 0) {
			ScoredRegion^ nextRegion = currentRegions.Max;
			PushToBuffer(nextRegion);
			currentRegions.Remove(nextRegion);
		}
		//std::cout << "After: " << buffer.BufferTime_us() << "us, " << buffer.Size() << " in buffer, " << currentRegions.Count << " current\n";
	}

	void InitRegions() {
		for (int x = 0; x < ArduinoScreen::width; x++) {
			for (int y = 0; y < ArduinoScreen::height; y++) {
				initialRegions[x, y] = gcnew ScoredRegion(x, y, 1, 1);
			}
		}
	}

	void PushAllPixels() {
		for (int x = 0; x < ArduinoScreen::width; x++) {
			for (int y = 0; y < ArduinoScreen::height; y++) {
				PushToBuffer(initialRegions[x, y]);
			}
		}
	}

	void PerformanceTest() {
		auto time = clock();
		PushAllPixels();
		while (buffer.BufferTime_us() < minBufferTime_us) {
			Sleep(1);
		}
		time = (clock() - time) * 1000;
		auto regionTime = time / (ArduinoScreen::width * ArduinoScreen::height);
		std::cout << "Total time: " << time << " us, region time: " << regionTime << " us\n";
	}

public:
	ArduinoScreen arduinoScreen;
	ThreadSafeQueue<ArduinoScreen^>^ frameQueue;
	ArduinoBuffer buffer;

	RegionFinder(ThreadSafeQueue<ArduinoScreen^>^ frameQueue) : ThreadWrapper("RegionFinder"), frameQueue(frameQueue),
		arduinoScreen(gcnew Color16(0, 0, 31)), currentRegions(gcnew ScoredRegionComparer()) {
		regionArray = gcnew array<ScoredRegion^, 2>(ArduinoScreen::width, ArduinoScreen::height);
		initialRegions = gcnew array<ScoredRegion^, 2>(ArduinoScreen::width, ArduinoScreen::height);
		InitRegions();
	}

protected:
	// Odziedziczono za poœrednictwem elementu ThreadWrapper
	void ThreadLoop() override
	{
		while (running)
		{
			ArduinoScreen^ target;
			frameQueue->TryPop(target);
			if (target) {
				ProcessFrame(target);

				//PerformanceTest();
				PushRegionsIfNeeded();
			}

			Sleep(1);
		}
	}
};
