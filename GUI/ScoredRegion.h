#pragma once
#include "Region.h"

using namespace System::Collections::Generic;

ref class ScoredRegion : public Region {
private:
	static int Pow2(int v) {
		return v * v;
	}

	static Color16^ CalcColor(Color16^ a, Color16^ b) {
		return gcnew Color16((a->r + b->r) / 2, (a->g + b->g) / 2, (a->b + b->b) / 2);
	}

	void CalcScore(Color16^ arduinoColor) {
		score = Pow2(color->r - arduinoColor->r)
			  + Pow2(color->g - arduinoColor->g)
			  + Pow2(color->b - arduinoColor->b);
	}

	void AddColorScore(Color16^ a, Color16^ b, int factor) {
		score += Pow2(a->r - b->r) * factor
			   + Pow2(a->g - b->g) * factor
			   + Pow2(a->b - b->b) * factor;
	}

	void CalcScore(ScoredRegion^ a, ScoredRegion^ b) {
		score = a->score + b->score;
		int mult = -a->sizeX * a->sizeY;
		AddColorScore(color, a->color, mult);
		AddColorScore(color, b->color, mult);
	}

	void SetPriority() {
		priority = score / float(drawingTime_us);
	}

public:
	int score = 0;
	float priority = 0;

	ScoredRegion(int x, int y, Color16^ targetColor, Color16^ arduinoColor) : Region(x, y, 1, 1, targetColor) {
		CalcScore(arduinoColor);
		SetPriority();
	}

	ScoredRegion(int x, int y, int sizeX, int sizeY, ScoredRegion^ a, ScoredRegion^ b) :
		Region(x, y, sizeX, sizeY, CalcColor(a->color, b->color)) {
		CalcScore(a, b);
		SetPriority();
	}
};