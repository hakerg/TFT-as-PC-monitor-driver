#pragma once
#include <vector>

template <class T, int width, int height>
class Array2D : private std::vector<T> {
public:
	Array2D() : std::vector<T>(width * height) {
		resize(size_t(width) * height);
	}

	T& At(int x, int y) {
		return at(size_t(y) * width + x);
	}

	T* Data() {
		return data();
	}
};