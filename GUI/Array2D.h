#pragma once

template <class T>
ref class Array2D
{
public:

	const unsigned sizeX;
	const unsigned sizeY;
	T* const data;

	Array2D(unsigned sizeX, unsigned sizeY);
	virtual ~Array2D();

	T& At(unsigned indexX, unsigned indexY);

};

template<class T>
inline Array2D<T>::Array2D(unsigned sizeX, unsigned sizeY) :
	sizeX(sizeX), sizeY(sizeY), data(new T[sizeX * sizeY])
{
}

template<class T>
inline Array2D<T>::~Array2D()
{
	delete[] data;
}

template<class T>
inline T& Array2D<T>::At(unsigned indexX, unsigned indexY)
{
	return data[indexX + sizeX * indexY];
}
