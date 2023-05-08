#pragma once
#include "ArduinoBuffer.h"
#include "ArduinoScreen.h"
#include "Color16.h"
#include "Region.h"
#include "RegionError.h"
#include "ThreadSafeQueue.h"
#include "ThreadWrapper.h"
#include <algorithm>
#include <ctime>
#include <vector>

using namespace System;
using namespace System::Threading;
using namespace System::Collections::Generic;

ref class RegionErrorComparer : public IComparer<RegionError^>
{
public:
	virtual int Compare(RegionError^ a, RegionError^ b)
	{
		if (a->priority != b->priority)
		{
			return a->priority > b->priority ? 1 : -1;
		}

		if (a->size != b->size)
		{
			return a->size > b->size ? -1 : 1;
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

	array<RegionError^, 2>^ initialRegions;
	array<RegionError^, 2>^ regionArray;
	SortedSet<RegionError^> currentRegions;

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
				regionArray[x, y]->SetPriority(target->At(x, y), arduinoScreen.At(x, y));
			}
		}
	}

	void MergeRegions() {
		for (int size = 1; size < ArduinoScreen::height; size *= 2) {
			for (int x = 0; x + size < ArduinoScreen::width; x += size * 2) {
				for (int y = 0; y + size < ArduinoScreen::height; y += size * 2) {
					RegionError^ a = regionArray[x, y];
					RegionError^ b = regionArray[x + size, y];
					RegionError^ c = regionArray[x, y + size];
					RegionError^ d = regionArray[x + size, y + size];
					if (a->size != size || b->size != size || c->size != size || d->size != size) {
						continue;
					}
					RegionError^ merged = gcnew RegionError(a, b, c, d);
					if (merged->priority > a->priority &&
						merged->priority > b->priority &&
						merged->priority > c->priority &&
						merged->priority > d->priority) {
						regionArray[x, y] = merged;
						a->priority = 0;
						b->priority = 0;
						c->priority = 0;
						d->priority = 0;
					}
				}
			}
		}
	}

	void PrepareCurrentRegions() {
		currentRegions.Clear();
		for (int x = 0; x < ArduinoScreen::width; x++) {
			for (int y = 0; y < ArduinoScreen::height; y++) {
				RegionError^ region = regionArray[x, y];
				if (region->priority > 0) {
					currentRegions.Add(region);
				}
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
		buffer.Push(region);
		UpdateArduinoScreen(region);
	}

	void PushRegionsIfNeeded() {
		//std::cout << "Before: " << buffer.BufferTime_us() << "us, " << buffer.Size() << " in buffer, " << currentRegions.Count << " current\n";
		while (buffer.BufferTime_us() < minBufferTime_us && currentRegions.Count > 0) {
			RegionError^ nextRegion = currentRegions.Max;
			PushToBuffer(nextRegion);
			currentRegions.Remove(nextRegion);
		}
		//std::cout << "After: " << buffer.BufferTime_us() << "us, " << buffer.Size() << " in buffer, " << currentRegions.Count << " current\n";
	}

	void InitRegions() {
		for (int x = 0; x < ArduinoScreen::width; x++) {
			for (int y = 0; y < ArduinoScreen::height; y++) {
				initialRegions[x, y] = gcnew RegionError(x, y);
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

	RegionFinder(ThreadSafeQueue<ArduinoScreen^>^ frameQueue) : frameQueue(frameQueue), arduinoScreen(gcnew Color16(0, 0, 31)),
		currentRegions(gcnew RegionErrorComparer()) {
		regionArray = gcnew array<RegionError^, 2>(ArduinoScreen::width, ArduinoScreen::height);
		initialRegions = gcnew array<RegionError^, 2>(ArduinoScreen::width, ArduinoScreen::height);
		InitRegions();
	}

protected:
	// Odziedziczono za poœrednictwem elementu ThreadWrapper
	void ThreadLoop() override
	{
		while (running)
		{
			ArduinoScreen^ target;
			if ((frameQueue->TryPop(target) || currentRegions.Count == 0) && target) {
				ProcessFrame(target);
			}

			//PerformanceTest();
			PushRegionsIfNeeded();

			Sleep(1);
		}
	}
};
