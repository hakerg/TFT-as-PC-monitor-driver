#pragma once
#include "Region.h"
#include "ThreadSafeQueue.h"
#include <cmath>

ref class ArduinoBuffer {
private:
	ThreadSafeQueue<Region^> queue;
	int bufferTime_us = 0;

	void FillData(Region^ region, uint8_t* data) {
		data[0] = region->x & 0xFF;
		data[1] = region->y;
		data[2] = (region->x >> 8) |
			(((int)log2(region->size) & 0xF) << 1) |
			((int)log2(region->size) << 5);
		*(uint16_t*)&data[3] = region->color.To565();
	}

public:
	void Push(Region^ region) {
		queue.WaitAndPush(region);
		Interlocked::Add(bufferTime_us, region->drawingTime_us);
	}

	void Pop(uint8_t* data) {
		Region^ region = queue.WaitAndPop();
		//std::cout << "Sending " << region->x << "x" << region->y << ", " << region->size << std::endl;
		FillData(region, data);
		Interlocked::Add(bufferTime_us, -region->drawingTime_us);
	}

	int Size() {
		return queue.Size();
	}

	int BufferTime_us() {
		return bufferTime_us;
	}
};