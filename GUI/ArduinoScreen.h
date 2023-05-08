#pragma once
#include "Color16.h"

using namespace System::Collections::Generic;

ref class ArduinoScreen {
private:
	array<Color16, 2>^ data;

public:
	static constexpr int width = 320;
	static constexpr int height = 240;

	ArduinoScreen() {
		data = gcnew array<Color16, 2>(width, height);
	}

	ArduinoScreen(Color16^ fill) {
		data = gcnew array<Color16, 2>(width, height);

		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				At(x, y) = *fill;
			}
		}
	}

	Color16% At(int x, int y) {
		return data[x, y];
	}
};
