#pragma once
#include "Color16.h"

using namespace System::Collections::Generic;

ref class ArduinoScreen {
private:
	array<Color16^, 2>^ data;

public:
	static constexpr int width = 320;
	static constexpr int height = 240;

	ArduinoScreen()
	{
		data = gcnew array<Color16^, 2>(width, height);

		Color16 fill;
		fill.r = 0;
		fill.g = 0;
		fill.b = 31;
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				At(x, y) = gcnew Color16(%fill);
			}
		}
	}

	virtual ~ArduinoScreen() {}

	Color16^% At(int x, int y) {
		return data[x, y];
	}
};
