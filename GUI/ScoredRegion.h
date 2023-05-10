#pragma once
#include "Region.h"
#include "ScoredRegionCalculation.h"

using namespace System::Collections::Generic;

ref class ScoredRegion : public Region {
public:
	ScoredRegionCalculation calc;

	ScoredRegion(int x, int y, int sizeX, int sizeY) : Region(x, y, sizeX, sizeY) {}

	ScoredRegion(int x, int y, int sizeX, int sizeY, ScoredRegionCalculation calc) : Region(x, y, sizeX, sizeY), calc(calc) {
		color = calc.color;
	}

	void InitPriority(Color16^ targetColor, Color16^ arduinoColor) {
		color = *targetColor;
		calc.InitPriority(targetColor, arduinoColor, drawingTime_us);
	}
};