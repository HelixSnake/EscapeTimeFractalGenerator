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
	this->complexBuffer = new ComplexFloat[width * height];
	this->rowOrColumnStorage = new ComplexFloat[std::max(width, height)];
}

FractalFourier::~FractalFourier()
{
	delete[] complexBuffer;
	delete[] rowOrColumnStorage;
}

void FractalFourier::Resize(int width, int height, double sizeMult)
{
	delete[] complexBuffer;
	delete[] rowOrColumnStorage;
	complexBuffer = new ComplexFloat[width * sizeMult * height * sizeMult];
	complexBufferWidth = width * sizeMult;
	complexBufferHeight = height * sizeMult;
	this->rowOrColumnStorage = new ComplexFloat[std::max(width * sizeMult, height * sizeMult)];
}

void FractalFourier::FillFromBuffer(const CF_Float* buffer, int bufferLength)
{
	int length = std::min(bufferLength, (int)(this->complexBufferWidth * this->complexBufferHeight));
	for (int i = 0; i < length; i++)
	{
		complexBuffer[i] = ComplexFloat(buffer[i], 0);
	}
}

void FractalFourier::ExecuteRowOrColumnRecursive(bool inverted, int length, int rowOrColumn, 
	GetIndexFunc &getIndexFunction)
{
	// approximate a split into a 2D array; quick algorithm to find dimensions closest to each other
	bool isPrime = false;
	int factor = 1;
	int divisor = length;
	while (factor < divisor && !isPrime)
	{
		isPrime = true;
		for (unsigned int prime : primes)
		{
			if (length % prime == 0)
			{
				factor *= prime;
				divisor /= prime;
				isPrime = false;
				break;
			}
		}
	}
	if (true) //core loop
	{
		for (int i = 0; i < length; i++)
		{
			rowOrColumnStorage[i] = ComputePoint(inverted, length, rowOrColumn, i, getIndexFunction);
		}
		for (int i = 0; i < length; i++)
		{
			complexBuffer[getIndexFunction(i, rowOrColumn, complexBufferWidth)] = rowOrColumnStorage[i];
		}
	}
	else
	{
		int matrixHeight = divisor;
		int matrixWidth = factor;
		// pretend the row is split into a 2D matrix with dimensions factor x divisor laid out like this:
		//0 3 6
		//1 4 7
		//2 5 8
		for (int y = 0; y < matrixHeight; y++) // rows
		{
			int row = y;
			int bufferHeight = matrixHeight;
			int rowLength = matrixWidth;
			GetIndexFunc nextIndexFunction = GetIndexFunc::MakeInstance(&getIndexFunction,
				[](const GetIndexFunc* parentFunction, int n, int row, int bufferHeight) {
					int index = n * bufferHeight + row;
					int parentsRowOrColumn = std::get<1>(parentFunction->inputStorage);
					int parentsBufferWidth = std::get<2>(parentFunction->inputStorage);
					return (*parentFunction)(index, parentsRowOrColumn, parentsBufferWidth); // run the parent function from the parent's row/column with our index
				});
			nextIndexFunction.inputStorage = { 0, row, bufferHeight };
			ExecuteRowOrColumnRecursive(inverted, rowLength, row,
				nextIndexFunction);
		}
		for (int x = 0; x < matrixWidth; x++) // columns
		{
			int column = x;
			int bufferHeight = matrixHeight;
			int columnLength = matrixHeight;
			GetIndexFunc nextIndexFunction = GetIndexFunc::MakeInstance(&getIndexFunction,
				[](const GetIndexFunc* parentFunction, int n, int column, int bufferHeight) {
					int index = n + column * bufferHeight;
					int parentsRowOrColumn = std::get<1>(parentFunction->inputStorage);
					int parentsBufferWidth = std::get<2>(parentFunction->inputStorage);
					return (*parentFunction)(index, parentsRowOrColumn, parentsBufferWidth); // run the parent function from the parent's row/column with our index
				});
			nextIndexFunction.inputStorage = { 0, column, bufferHeight };
			ExecuteRowOrColumnRecursive(inverted, columnLength, column,
				nextIndexFunction);
		}
	}
}

ComplexFloat FractalFourier::ComputePoint(bool inverted, int length, int rowOrColumn, int currentIndex,
	GetIndexFunc &getIndexFunction)
{
	ComplexFloat sum = ComplexFloat(0, 0);
	CF_Float constCore = currentIndex * 2 * PI_CONSTANT / length;
	for (int i = 0; i < length; i++)
	{
		ComplexFloat fk = complexBuffer[getIndexFunction(i, rowOrColumn, complexBufferWidth)];
		CF_Float core = constCore * i;
		if (inverted) core = -core;
		sum = sum + fk * ComplexFloat(cosl(core), -sinl(core));
	}
	if (inverted) return sum / length;
	else return sum;
}

void FractalFourier::Execute(bool inverted) // fast fourier transform
{
	GetIndexFunc getIndexFunctionRows = GetIndexFunc::MakeInstance(nullptr,
		[](const GetIndexFunc* parentFunction, int n, int row, int bufferWidth) {return n + row * bufferWidth; });
	for (int y = 0; y < complexBufferHeight; y++)
	{
		int row = y;
		int bufferWidth = complexBufferWidth;
		int rowLength = complexBufferWidth;
		getIndexFunctionRows.inputStorage = { 0, row, bufferWidth };
		ExecuteRowOrColumnRecursive(inverted, rowLength, row, getIndexFunctionRows);
	}
	GetIndexFunc getIndexFunctionColumns = GetIndexFunc::MakeInstance(nullptr,
		[](const GetIndexFunc* parentFunction, int n, int column, int bufferWidth) {return column + n * bufferWidth; });
	for (int x = 0; x < complexBufferWidth; x++)
	{
		int column = x;
		int bufferWidth = complexBufferWidth;
		int columnLength = complexBufferHeight;
		getIndexFunctionColumns.inputStorage = { 0, column, bufferWidth };
		ExecuteRowOrColumnRecursive(inverted, columnLength, column, getIndexFunctionColumns);
	}
}

void FractalFourier::ClearImaginary()
{
	for (int i = 0; i < complexBufferWidth; i++)
	{
		for (int j = 0; j < complexBufferHeight; j++)
		{
			complexBuffer[i + j * complexBufferWidth].imaginary = 0;
		}
	}
}

void FractalFourier::Magnitude()
{
	for (int i = 0; i < complexBufferWidth; i++)
	{
		for (int j = 0; j < complexBufferHeight; j++)
		{
			complexBuffer[i + j * complexBufferWidth].real = complexBuffer[i + j * complexBufferWidth].AbsoluteValue();
		}
	}
}

void FractalFourier::ClearReal()
{
	for (int i = 0; i < complexBufferWidth; i++)
	{
		for (int j = 0; j < complexBufferHeight; j++)
		{
			complexBuffer[i + j * complexBufferWidth].real = 0;
		}
	}
}

void FractalFourier::Timesi()
{
	for (int i = 0; i < complexBufferWidth; i++)
	{
		for (int j = 0; j < complexBufferHeight; j++)
		{
			complexBuffer[i + j * complexBufferWidth] = ComplexFloat::MultByi(complexBuffer[i + j * complexBufferWidth]);
		}
	}
}

void FractalFourier::CopyBuffer(CF_Float* dest, int bufferSize)
{
	int length = bufferSize;
	length = std::min(length, (int)(complexBufferHeight * complexBufferWidth));
	for (int i = 0; i < length; i++)
	{
		dest[i] = complexBuffer[i].real;
	}
}

void FractalFourier::GetBufferDimensions(int& bufferWidth, int& bufferHeight)
{
	bufferWidth = complexBufferWidth;
	bufferHeight = complexBufferHeight;
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