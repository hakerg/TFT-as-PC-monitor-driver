#include "ScoredRegionCalculation.h"
#include "ScoredRegion.h"

inline void ScoredRegionCalculation::CalcScore(ScoredRegion^ a, ScoredRegion^ b) {
	scoreR = a->calc.scoreR + b->calc.scoreR;
	scoreG = a->calc.scoreG + b->calc.scoreG;
	scoreB = a->calc.scoreB + b->calc.scoreB;
	int mult = -a->sizeX * a->sizeY;
	AddColorScore(color, a->color, mult);
	AddColorScore(color, b->color, mult);
}

ScoredRegionCalculation::ScoredRegionCalculation(ScoredRegion^ a, ScoredRegion^ b) {
	CalcColor(a->color, b->color);
	CalcScore(a, b);
	SetPriority(Region::GetDrawingTime_us(a->sizeX * a->sizeY * 2));
}
