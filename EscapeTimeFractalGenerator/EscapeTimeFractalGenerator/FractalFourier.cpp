#include "FractalFourier.h"
#include <math.h>
#include <algorithm>

const long double E_CONSTANT = 2.71828182845904523536028747135266249775724709369995;
const long double PI_CONSTANT = 3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117068;

const unsigned int FractalFourier::primes[3] = { 2, 3, 5 }; // more primes means more possible ideal dimensions but slower fft

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

ComplexFloat FractalFourier::ExecutePoint(bool inverted, int length, int rowOrColumn, int currentIndex, ComplexFloat* source, 
	int(*getCFIndex)(int n, int rowOrColumn, int rowWidth))
{
	ComplexFloat sum = ComplexFloat(0,0);
	for (int i = 0; i < length; i++)
	{
		ComplexFloat fk = source[getCFIndex(i, rowOrColumn, complexBufferWidth)];
		CF_Float kOverN = (CF_Float)i / length;
		CF_Float core = currentIndex * 2 * PI_CONSTANT * kOverN;
		if (inverted) core = -core;
		sum = sum + fk * ComplexFloat(cosl(core), -sinl(core));
	}
	if (inverted) return sum / length;
	else return sum;
}

void FractalFourier::Execute(bool inverted) // fast fourier transform
{
	std::swap(currentBuffer, otherBuffer);
	for (int i = 0; i < complexBufferWidth; i++)
	{
		for (int j = 0; j < complexBufferHeight; j++)
		{
			(*currentBuffer)[i + j * complexBufferWidth] = ExecutePoint(inverted, complexBufferWidth, j, i, *otherBuffer,
				[](int n, int roworcolumn, int width) {return n + roworcolumn * width; });
		}
	}
	std::swap(currentBuffer, otherBuffer);
	for (int i = 0; i < complexBufferWidth; i++)
	{
		for (int j = 0; j < complexBufferHeight; j++)
		{
			(*currentBuffer)[i + j * complexBufferWidth] = ExecutePoint(inverted, complexBufferHeight, i, j, *otherBuffer,
				[](int n, int roworcolumn, int width) {return roworcolumn + n * width; });
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

unsigned int FractalFourier::FindClosestIdealFactorization(unsigned int input)
{
	int i = 0;
	bool solved = false;
	while (!solved) // guaranteed to break eventually, since as the while loop goes on newinput will work its way towards 0 and is guaranteed to hit a candidate
	{
		int newInput = input + i;
		if (newInput < 1) return 0; // this should never happen but we need this edge case to prevent infinite loops
		bool notCandidate = false;
		while (!solved && !notCandidate) // guaranteed to exit eventually since we will either find a solution or prove it's not a solution
		{
			bool foundPrime = false;
			for (unsigned int prime : primes)
			{
				if (newInput == 1) // prime factorization includes only primes from our list
				{
					solved = true;
					return input + i;
				}
				if (newInput % prime == 0)
				{
					newInput /= prime;
					foundPrime = true; // continue the loop
					break;
				}
			}
			if (!foundPrime) // our number is not a candidate since it has a prime factor outside our list of primes
				notCandidate = true;
		}
		i = i < 1 ? -i + 1 : -i; // intended sequence: 0 1 -1 2 -2 3 -3 4 -4 etc.
	}
}