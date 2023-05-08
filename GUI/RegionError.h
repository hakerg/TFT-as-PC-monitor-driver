#pragma once
#include "Region.h"

using namespace System::Collections::Generic;

ref class RegionError : public Region {
private:
	int scoreR = 0;
	int scoreG = 0;
	int scoreB = 0;

	static inline int Pow2(int v) {
		return v * v;
	}

	void SetPriority() {
		priority = (scoreR + scoreG + scoreB) / float(drawingTime_us);
	}

	void CalcColor(RegionError^ a, RegionError^ b, RegionError^ c, RegionError^ d) {
		color.r = (a->color.r + b->color.r + c->color.r + d->color.r) / 4;
		color.g = (a->color.g + b->color.g + c->color.g + d->color.g) / 4;
		color.b = (a->color.b + b->color.b + c->color.b + d->color.b) / 4;
	}

	void AddColorError(Color16^ a, Color16^ b, int factor) {
		scoreR += Pow2(a->r - b->r) * factor;
		scoreG += Pow2(a->g - b->g) * factor;
		scoreB += Pow2(a->b - b->b) * factor;
	}

public:
	float priority = 0;

	RegionError(int x, int y) : Region(x, y) {}

	void SetPriority(Color16^ targetColor, Color16^ arduinoColor) {
		color = *targetColor;

		scoreR = scoreG = scoreB = 0;
		AddColorError(targetColor, arduinoColor, 1);

		SetPriority();
	}

	RegionError(RegionError^ a, RegionError^ b, RegionError^ c, RegionError^ d) : Region(a->x, a->y, a->size * 2) {
		CalcColor(a, b, c, d);

		int mult = -(a->size * a->size);
		scoreR = a->scoreR + b->scoreR + c->scoreR + d->scoreR;
		scoreG = a->scoreG + b->scoreG + c->scoreG + d->scoreG;
		scoreB = a->scoreB + b->scoreB + c->scoreB + d->scoreB;
		AddColorError(% color, % a->color, mult);
		AddColorError(% color, % b->color, mult);
		AddColorError(% color, % c->color, mult);
		AddColorError(% color, % d->color, mult);

		SetPriority();
	}
};