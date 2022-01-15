#include "FractalFourier.h"
#include <math.h>
#include <algorithm>

const long double E_CONSTANT = 2.71828182845904523536028747135266249775724709369995;
const long double PI_CONSTANT = 3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117068;

FractalFourier::FractalFourier(int width, int height)
{
	this->complexBufferWidth = width;
	this->complexBufferHeight = height;
	this->complexBuffer = new ComplexFloat[width * height];
	this->rowStorage1 = new ComplexFloat[std::max(width, height)];
	this->rowStorage2 = new ComplexFloat[std::max(width, height)];
	this->chunkStorageSource = new ComplexFloat[MAX_FINAL_CHUNK_SIZE];
	this->chunkStorageDest = new ComplexFloat[MAX_FINAL_CHUNK_SIZE];
}

FractalFourier::~FractalFourier()
{
	delete[] complexBuffer;
	delete[] rowStorage1;
	delete[] rowStorage2;
	delete[] chunkStorageSource;
	delete[] chunkStorageDest;
}

void FractalFourier::Resize(int width, int height, double sizeMult)
{
	delete[] complexBuffer;
	delete[] rowStorage1;
	delete[] rowStorage2;
	complexBuffer = new ComplexFloat[width * sizeMult * height * sizeMult];
	complexBufferWidth = width * sizeMult;
	complexBufferHeight = height * sizeMult;
	rowStorage1 = new ComplexFloat[std::max(width * sizeMult, height * sizeMult)];
	rowStorage2 = new ComplexFloat[std::max(width * sizeMult, height * sizeMult)];
}

void FractalFourier::FillFromBuffer(const CF_Float* buffer, int bufferLength)
{
	int length = std::min(bufferLength, (int)(this->complexBufferWidth * this->complexBufferHeight));
	for (int i = 0; i < length; i++)
	{
		complexBuffer[i] = ComplexFloat(buffer[i], 0);
	}
}
void FractalFourier::SwapStorageBuffers()
{
	std::swap(sourceRowStorage, destRowStorage);
}

void FractalFourier::ExecuteRowOrColumnSlow(bool inverted, int length)
{
	for (int i = 0; i < length; i++)
	{
		ComplexFloat sum = ComplexFloat(0, 0);
		CF_Float constCore = -i * 2 * PI_CONSTANT / length;
		for (int j = 0; j < length; j++)
		{
			ComplexFloat fk = (*sourceRowStorage)[j];
			CF_Float core = constCore * j;
			if (inverted) core = -core;
			sum = sum + fk * ComplexFloat(cosl(core), sinl(core));
		}
		if (inverted) (*destRowStorage)[i] = sum / length;
		else (*destRowStorage)[i] = sum;
	}
	SwapStorageBuffers();
}

void FractalFourier::ExecuteRowOrColumn(bool inverted, int length) // fft
{
	ReorderStorage(length); // reorder storage
	unsigned int chunkLength = length;
	while ((chunkLength & 1) == 0)
		chunkLength >>= 1;
	for (int i = 0; i < length / chunkLength; i++) // evaluate all the smallest chunks whose length is not a multiple of 2
	{
		for (int j = 0; j < chunkLength; j++)
		{
			chunkStorageSource[j] = (*sourceRowStorage)[j + i * chunkLength];
		}
		ExecuteFinalChunk(inverted, chunkLength);
		for (int j = 0; j < chunkLength; j++)
		{
			(*destRowStorage)[j + i * chunkLength] = chunkStorageDest[j];
		}
	}
	SwapStorageBuffers();
	while (chunkLength != length) // log 2 complexity
	{
		unsigned int halfChunkLength = chunkLength;
		chunkLength <<= 1;
		CF_Float twiddleConst = -2 * PI_CONSTANT / chunkLength;
		//ReorderStorage(length, true, false, chunkLength); // reverse reorder storage one step
		for (int i = 0; i < length / chunkLength; i++) // for each chunk
		{
			int chunkStart = i * chunkLength;
			for (int k = 0; k < halfChunkLength; k++) //both loops = linear complexity
			{

				int firstHalfIndex = k + chunkStart;
				int secondHalfIndex = k + halfChunkLength + chunkStart;
				CF_Float twiddleFactorCore = k * twiddleConst;
				if (inverted)
				{
					twiddleFactorCore = -twiddleFactorCore;
				}
				ComplexFloat twiddleFactor = ComplexFloat(cosl(twiddleFactorCore), sinl(twiddleFactorCore));
				ComplexFloat Ek = (*sourceRowStorage)[firstHalfIndex]; // evens
				ComplexFloat Ok = (*sourceRowStorage)[secondHalfIndex]; // odds
				(*destRowStorage)[firstHalfIndex] = Ek + twiddleFactor * Ok;
				(*destRowStorage)[secondHalfIndex] = Ek - twiddleFactor * Ok;
			}
			//std::cout << std::endl;
		}
		//std::cout << std::endl;
		SwapStorageBuffers();
	}
	if (inverted)
	{
		for (int i = 0; i < length; i++)
		{
			//just operate on the source so we don't have to swap
			(*sourceRowStorage)[i] = (*sourceRowStorage)[i] / length;
		}
	}
}
//00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 oldlen = 16
//00 02 04 06 08 10 12 14|01 03 05 07 09 11 13 15 oldlen = 8
//00 04 08 12|02 06 10 14|01 05 09 13|03 07 11 15 oldlen = 4
//00 08|04 12|02 10|06 14|01 09|05 13|03 11|07 15 oldlen = 2
//etc.
void FractalFourier::ReorderStorage(int length) // complexity: nlogn
{
	//pseudocode:
	//length = length of row
	// while length is even
		// chunkOffset = floor oldindex to nearest multiple of oldlength = oldindex - oldindex % oldlength
		// newlength = oldlength / 2
		// oldIndex -= chunkOffset
		// if oldindex is even, move to the second half of the next chunk (add newlength) and then add the old index / 2
		// newindex = oldindex % 2 * newlength + oldindex / 2
		// newindex += chunkOffset

	for (int i = 0; i < length; i++)
	{
		unsigned int newLength = length;
		unsigned int index = i;
		//std::cout << index << " ";
		while ((newLength & 1) == 0)
		{
			int offset = index - index % newLength;
			newLength >>= 1;
			index -= offset;
			//index = index % 2 * newLength + index / 2;
			index = (index & 1) == 0 ? index >> 1 : newLength + (index >> 1);
			index += offset;
		}
		//std::cout << std::endl;
		(*destRowStorage)[index] = (*sourceRowStorage)[i];
	}
	SwapStorageBuffers();
}

void FractalFourier::ExecuteFinalChunk(bool inverted, int length) // O(n^2) for the chunk size
{
	if (length == 1)
	{
		chunkStorageDest[0] = chunkStorageSource[0];
	}
	for (int i = 0; i < length; i++)
	{
		ComplexFloat sum = ComplexFloat(0, 0);
		CF_Float constCore = -i * 2 * PI_CONSTANT / length;
		for (int j = 0; j < length; j++)
		{
			ComplexFloat fk = chunkStorageSource[j];
			CF_Float core = constCore * j;
			if (inverted) core = -core;
			sum = sum + fk * ComplexFloat(cosl(core), sinl(core));
		}
		chunkStorageDest[i] = sum;
	}
}

void FractalFourier::Execute(bool inverted) // fast fourier transform
{
	for (int y = 0; y < complexBufferHeight; y++)
	{
		int bufferWidth = complexBufferWidth;
		int rowLength = complexBufferWidth;
		int stride = y * bufferWidth;
		for (int i = 0; i < rowLength; i++)
			(*destRowStorage)[i] = complexBuffer[i + stride];
		SwapStorageBuffers();
		ExecuteRowOrColumn(inverted, rowLength);
		for (int i = 0; i < rowLength; i++)
			complexBuffer[i + stride] = (*sourceRowStorage)[i];
	}

	for (int x = 0; x < complexBufferWidth; x++)
	{
		int bufferWidth = complexBufferWidth;
		int columnHeight = complexBufferHeight;
		int strideOffset = 0;
		for (int i = 0; i < columnHeight; i++)
		{
			(*destRowStorage)[i] = complexBuffer[x + strideOffset];
			strideOffset += bufferWidth;
		}
		SwapStorageBuffers();
		ExecuteRowOrColumn(inverted, columnHeight);
		strideOffset = 0;
		for (int i = 0; i < columnHeight; i++)
		{
			complexBuffer[x + strideOffset] = (*sourceRowStorage)[i];
			strideOffset += bufferWidth;
		}
	}
}

void FractalFourier::RebaseBuffer()
{

	for (int y = 0; y < complexBufferHeight; y++)
	{
		int bufferWidth = complexBufferWidth;
		int rowLength = complexBufferWidth;
		for (int i = 0; i < rowLength; i++)
			(*destRowStorage)[i] = complexBuffer[i + y * bufferWidth];
		SwapStorageBuffers();
		for (int i = 0; i < rowLength; i++)
			complexBuffer[i + y * bufferWidth] = (*sourceRowStorage)[(i + rowLength / 2) % rowLength];
	}

	for (int x = 0; x < complexBufferWidth; x++)
	{
		int bufferWidth = complexBufferWidth;
		int columnHeight = complexBufferHeight;
		for (int i = 0; i < columnHeight; i++)
			(*destRowStorage)[i] = complexBuffer[x + i * bufferWidth];
		SwapStorageBuffers();
		for (int i = 0; i < columnHeight; i++)
			complexBuffer[x + i * bufferWidth] = (*sourceRowStorage)[(i+ columnHeight/2) % columnHeight];
	}
}

void FractalFourier::Magnitude()
{
	for (int i = 0; i < complexBufferWidth; i++)
	{
		for (int j = 0; j < complexBufferHeight; j++)
		{
			complexBuffer[i + j * complexBufferWidth].real = complexBuffer[i + j * complexBufferWidth].AbsoluteValue();
			complexBuffer[i + j * complexBufferWidth].imaginary = 0;
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
		if (complexBuffer[i].real < 0.0000000001l)
			dest[i] = 0;
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
	while (true) // guaranteed to break eventually, since as the while loop goes on newinput will work its way towards 0 and is guaranteed to hit a candidate
	{
		int newInput = input + i;
		if (newInput < 1) return 0; // this should never happen but we need this edge case to prevent infinite loops
		while (newInput % 2 == 0)
		{
			newInput /= 2;
		}
		if (newInput <= MAX_FINAL_CHUNK_SIZE)
			return input + i;
		i = i < 1 ? -i + 1 : -i; // intended sequence: 0 1 -1 2 -2 3 -3 4 -4 etc.
	}
}