#pragma once
#include "Color16.h"
#include "Region.h"

ref class ScoredRegion;

value class ScoredRegionCalculation {
private:
	int scoreR;
	int scoreG;
	int scoreB;

	static int Pow2(int v) {
		return v * v;
	}

	void CalcColor(Color16^ a, Color16^ b) {
		color.r = (a->r + b->r) / 2;
		color.g = (a->g + b->g) / 2;
		color.b = (a->b + b->b) / 2;
	}

	void AddColorScore(Color16^ a, Color16^ b, int factor) {
		scoreR += Pow2(a->r - b->r) * factor;
		scoreG += Pow2(a->g - b->g) * factor;
		scoreB += Pow2(a->b - b->b) * factor;
	}

	void CalcScore(ScoredRegion^ a, ScoredRegion^ b);

	void SetPriority(int drawingTime_us) {
		priority = (scoreR + scoreG + scoreB) / float(drawingTime_us);
	}

public:
	float priority;
	Color16 color;

	ScoredRegionCalculation(ScoredRegion^ a, ScoredRegion^ b);

	void InitPriority(Color16^ targetColor, Color16^ arduinoColor, int drawingTime_us) {
		scoreR = Pow2(targetColor->r - arduinoColor->r);
		scoreG = Pow2(targetColor->g - arduinoColor->g);
		scoreB = Pow2(targetColor->b - arduinoColor->b);
		SetPriority(drawingTime_us);
	}
};