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
	void Execute();
	void CopyBuffer(CF_Float* dest, int bufferSize);
	void GetBufferDimensions(int& bufferWidth, int& bufferHeight);
	void Resize(int width, int height, double sizeMult);
private:
	ComplexFloat* complexBuffer1 = nullptr;
	ComplexFloat* complexBuffer2 = nullptr;
	ComplexFloat** currentBuffer = &complexBuffer1;
	ComplexFloat** otherBuffer = &complexBuffer2;
	unsigned int complexBufferHeight;
	unsigned int complexBufferWidth;
};