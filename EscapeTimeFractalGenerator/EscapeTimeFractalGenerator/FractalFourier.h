#pragma once
#include "ComplexFloat.h"
class FractalFourier
{
public:
	FractalFourier(int width, int height);
	~FractalFourier();
	FractalFourier(const FractalFourier& other) = delete;
	FractalFourier operator=(const FractalFourier& other) = delete;
	FractalFourier(FractalFourier&& other) = delete;
	FractalFourier operator=(FractalFourier&& other) = delete;

	void FillFromBuffer(const CF_Float* buffer, int bufferLength);
	void Execute(bool inverted);
	void ClearImaginary();
	void Magnitude();
	void ClearReal();
	void Timesi();
	void CopyBuffer(CF_Float* dest, int bufferSize);
	void GetBufferDimensions(int& bufferWidth, int& bufferHeight);
	void Resize(int width, int height, double sizeMult);
	static unsigned int FindClosestIdealFactorization(unsigned int input);
private:
	ComplexFloat* complexBuffer1 = nullptr;
	ComplexFloat* complexBuffer2 = nullptr;
	ComplexFloat** currentBuffer = &complexBuffer1;
	ComplexFloat** otherBuffer = &complexBuffer2;
	unsigned int complexBufferHeight;
	unsigned int complexBufferWidth;
	static const unsigned int primes[3]; // more higher primes = slower fft but more likely to find closer ideal factorization

	ComplexFloat ExecutePoint(bool inverted, int length, int rowOrColumn, int currentIndex, ComplexFloat* source, 
		int(*getCFindex)(int n, int rowOrColumn, int rowWidth));
};