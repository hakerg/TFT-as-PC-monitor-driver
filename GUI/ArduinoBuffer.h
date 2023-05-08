#pragma once
#include "Region.h"
#include "ThreadSafeQueue.h"

ref class ArduinoBuffer {
private:
	ThreadSafeQueue<Region^> queue;
	int bufferTime_us = 0;

public:
	void Push(Region^ region) {
		queue.WaitAndPush(region);
		Interlocked::Add(bufferTime_us, region->drawingTime_us);
	}

	void Pop(uint8_t* data) {
		Region^ region = queue.WaitAndPop();
		//std::cout << "Sending " << region->x << "x" << region->y << ", " << region->size << std::endl;
		region->FillData(data);
		Interlocked::Add(bufferTime_us, -region->drawingTime_us);
	}

	int Size() {
		return queue.Size();
	}

	int BufferTime_us() {
		return bufferTime_us;
	}
};