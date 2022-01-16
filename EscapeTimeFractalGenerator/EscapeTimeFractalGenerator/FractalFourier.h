#pragma once
#include "ComplexFloat.h"
#include "ThreadSafeBuffer.h"
#include <iostream>
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

	void Execute(bool inverted, ThreadSafeBuffer<ComplexFloat>* buffer);
	void Magnitude(ThreadSafeBuffer<ComplexFloat>* buffer);
	void RebaseBuffer(ThreadSafeBuffer<ComplexFloat>* buffer);
	//void CopyBuffer(CF_Float* dest, int bufferSize); // no! bad! waste of memory! don't do this
	static unsigned int FindClosestIdealFactorization(unsigned int input);
private:
	static const int MAX_FINAL_CHUNK_SIZE = 10;
	ComplexFloat* rowStorage1 = nullptr;
	ComplexFloat* rowStorage2 = nullptr;
	ComplexFloat** sourceRowStorage = &rowStorage1;
	ComplexFloat** destRowStorage = &rowStorage2;

	void ExecuteFinalChunk(bool inverted, int start, int length);
	void ExecuteRowOrColumn(bool inverted, int length);
	void ExecuteRowOrColumnSlow(bool inverted, int length);
	void ReorderStorage(int length);
	void SwapStorageBuffers();
};