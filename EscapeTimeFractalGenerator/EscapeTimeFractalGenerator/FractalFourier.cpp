#include "FractalFourier.h"
#include <math.h>
#include <algorithm>

FractalFourier::FractalFourier(int width, int height)
{
	this->complexBufferWidth = width;
	this->complexBufferHeight = height;
	this->complexBuffer1 = new ComplexFloat[width * height];
	this->complexBuffer2 = new ComplexFloat[width * height];
}

void FractalFourier::FillFromBuffer(const CF_Float* buffer, int bufferLength)
{
	int length = std::min(bufferLength, (int)(this->complexBufferWidth * this->complexBufferHeight));
	for (int i = 0; i < length; i++)
	{
		(*currentBuffer)[i] = ComplexFloat(buffer[i], 0);
	}
}
void FractalFourier::Execute() // Placeholder function
{
	std::swap(currentBuffer, otherBuffer);
	for (int i = 0; i < complexBufferWidth; i++)
	{
		for (int j = 0; j < complexBufferHeight; j++)
		{
			(*currentBuffer)[i + j * complexBufferWidth] = (*otherBuffer)[(i * 2 % complexBufferWidth + i * 2 / complexBufferWidth) + j * complexBufferWidth];
		}
	}
}
void FractalFourier::CopyBuffer(CF_Float* dest, int bufferSize)
{
	int length = bufferSize;
	length = std::min(length, (int)(complexBufferHeight * complexBufferWidth));
	for (int i = 0; i < length; i++)
	{
		dest[i] = (*currentBuffer)[i].real;
	}
}

void FractalFourier::GetBufferDimensions(int& bufferWidth, int& bufferHeight)
{
	bufferWidth = complexBufferWidth;
	bufferHeight = complexBufferHeight;
}

void FractalFourier::Resize(int width, int height, double sizeMult)
{
	delete[] complexBuffer1;
	delete[] complexBuffer2;
	complexBuffer1 = new ComplexFloat[width * sizeMult * height * sizeMult];
	complexBuffer2 = new ComplexFloat[width * sizeMult * height * sizeMult];
	complexBufferWidth = width * sizeMult;
	complexBufferHeight = height * sizeMult;
}

FractalFourier::~FractalFourier()
{
	delete[] complexBuffer1;
	delete[] complexBuffer2;
}