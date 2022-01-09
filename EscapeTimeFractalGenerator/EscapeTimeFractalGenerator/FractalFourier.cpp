#include "FractalFourier.h"
#include <math.h>
#include <algorithm>

const long double E_CONSTANT = 2.71828182845904523536028747135266249775724709369995;
const long double PI_CONSTANT = 3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117068;

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
void FractalFourier::Execute()
{
	std::swap(currentBuffer, otherBuffer);
	for (int i = 0; i < complexBufferWidth; i++)
	{
		for (int j = 0; j < complexBufferHeight; j++)
		{
			ComplexFloat sum = ComplexFloat(0, 0);
			for (int i2 = 0; i2 < complexBufferWidth; i2++)
			{
				ComplexFloat fk = (*otherBuffer)[i2 + j * complexBufferWidth];
				CF_Float kOverN = (CF_Float)i2 / complexBufferWidth;
				//sum = sum + fk * ComplexFloat::Power(E_CONSTANT, ComplexFloat(0, 1) * -2 * PI_CONSTANT * i * kOverN);
				CF_Float core = i * 2 * PI_CONSTANT * kOverN;
				sum = sum + fk * ComplexFloat(cosl(core), -sinl(core));
			}
			(*currentBuffer)[i + j * complexBufferWidth] = sum;
		}
	}
	std::swap(currentBuffer, otherBuffer);
	for (int i = 0; i < complexBufferWidth; i++)
	{
		for (int j = 0; j < complexBufferHeight; j++)
		{
			ComplexFloat sum = ComplexFloat(0, 0);
			for (int j2 = 0; j2 < complexBufferHeight; j2++)
			{
				ComplexFloat fk = (*otherBuffer)[i + j2 * complexBufferWidth];
				CF_Float kOverN = (CF_Float)j2 / complexBufferHeight;
				//sum = sum + fk * ComplexFloat::Power(E_CONSTANT, ComplexFloat(0, 1) * -2 * PI_CONSTANT * j * kOverN);
				CF_Float core = j * 2 * PI_CONSTANT * kOverN;
				sum = sum + fk * ComplexFloat(cosl(core), -sinl(core));
			}
			(*currentBuffer)[i + j * complexBufferWidth] = sum;
		}
	}
}

void FractalFourier::Reverse()
{
	std::swap(currentBuffer, otherBuffer);
	for (int i = 0; i < complexBufferWidth; i++)
	{
		for (int j = 0; j < complexBufferHeight; j++)
		{
			ComplexFloat sum = ComplexFloat(0, 0);
			for (int i2 = 0; i2 < complexBufferWidth; i2++)
			{
				ComplexFloat fn = (*otherBuffer)[i2 + j * complexBufferWidth];
				CF_Float kOverN = (CF_Float)i2 / complexBufferWidth;
				CF_Float core = -i * 2 * PI_CONSTANT * kOverN;
				sum = sum + fn * ComplexFloat(cosl(core), -sinl(core));
			}
			(*currentBuffer)[i + j * complexBufferWidth] = sum / complexBufferWidth;
		}
	}
	std::swap(currentBuffer, otherBuffer);
	for (int i = 0; i < complexBufferWidth; i++)
	{
		for (int j = 0; j < complexBufferHeight; j++)
		{
			ComplexFloat sum = ComplexFloat(0, 0);
			for (int j2 = 0; j2 < complexBufferHeight; j2++)
			{
				ComplexFloat fn = (*otherBuffer)[i + j2 * complexBufferWidth];
				CF_Float kOverN = (CF_Float)j2 / complexBufferHeight;
				//sum = sum + fk * ComplexFloat::Power(E_CONSTANT, ComplexFloat(0, 1) * -2 * PI_CONSTANT * j * kOverN);
				CF_Float core = -j * 2 * PI_CONSTANT * kOverN;
				sum = sum + fn * ComplexFloat(cosl(core), -sinl(core));
			}
			(*currentBuffer)[i + j * complexBufferWidth] = sum / complexBufferHeight;
		}
	}
}

void FractalFourier::ClearImaginary()
{
	for (int i = 0; i < complexBufferWidth; i++)
	{
		for (int j = 0; j < complexBufferHeight; j++)
		{
			(*currentBuffer)[i + j * complexBufferWidth].imaginary = 0;
		}
	}
}

void FractalFourier::Magnitude()
{
	for (int i = 0; i < complexBufferWidth; i++)
	{
		for (int j = 0; j < complexBufferHeight; j++)
		{
			(*currentBuffer)[i + j * complexBufferWidth].real = (*currentBuffer)[i + j * complexBufferWidth].AbsoluteValue();
		}
	}
}

void FractalFourier::ClearReal()
{
	for (int i = 0; i < complexBufferWidth; i++)
	{
		for (int j = 0; j < complexBufferHeight; j++)
		{
			(*currentBuffer)[i + j * complexBufferWidth].real = 0;
		}
	}
}

void FractalFourier::Timesi()
{
	for (int i = 0; i < complexBufferWidth; i++)
	{
		for (int j = 0; j < complexBufferHeight; j++)
		{
			(*currentBuffer)[i + j * complexBufferWidth] = ComplexFloat::MultByi((*currentBuffer)[i + j * complexBufferWidth]);
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