#pragma once
#include "ComplexFloat.h"
#include "RecursiveFunctor.h"
class FractalFourier
{
public:
	FractalFourier(int width, int height);
	~FractalFourier();
	FractalFourier(const FractalFourier& other) = delete;
	FractalFourier operator=(const FractalFourier& other) = delete;
	FractalFourier(FractalFourier&& other) = delete;
	FractalFourier operator=(FractalFourier&& other) = delete;
	void Resize(int width, int height, double sizeMult);

	void FillFromBuffer(const CF_Float* buffer, int bufferLength);
	void Execute(bool inverted);
	void ClearImaginary();
	void Magnitude();
	void ClearReal();
	void Timesi();
	void CopyBuffer(CF_Float* dest, int bufferSize);
	void GetBufferDimensions(int& bufferWidth, int& bufferHeight);
	static unsigned int FindClosestIdealFactorization(unsigned int input);
private:
	ComplexFloat* complexBuffer = nullptr;
	ComplexFloat* rowOrColumnStorage = nullptr;
	unsigned int complexBufferHeight;
	unsigned int complexBufferWidth;
	static const unsigned int primes[3]; // more higher primes = slower fft but more likely to find closer ideal factorization

	typedef RecursiveFunctor<int, int, int, int> GetIndexFunc; // output: index, inputs: n, rowOrColumn, bufferWidth

	ComplexFloat ComputePoint(bool inverted, int length, int rowOrColumn, int currentIndex,
		GetIndexFunc &getIndexFunction);
	void ExecuteRowOrColumnRecursive(bool inverted, int length, int rowOrColumn,
		GetIndexFunc &getIndexFunction);
};