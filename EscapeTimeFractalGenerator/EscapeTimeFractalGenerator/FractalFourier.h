#pragma once
#include "ComplexFloat.h"
#include "RecursiveFunctor.h"
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

	void FillFromBuffer(const CF_Float* buffer, int bufferLength);
	void Execute(bool inverted);
	void Reverse();
	void ClearImaginary();
	void Magnitude();
	void ClearReal();
	void Timesi();
	void CopyBuffer(CF_Float* dest, int bufferSize);
	void GetBufferDimensions(int& bufferWidth, int& bufferHeight);
	static unsigned int FindClosestIdealFactorization(unsigned int input);
private:
	static const int MAX_FINAL_CHUNK_SIZE = 10;
	ComplexFloat* complexBuffer = nullptr;
	ComplexFloat* rowStorage1 = nullptr;
	ComplexFloat* rowStorage2 = nullptr;
	ComplexFloat** sourceRowStorage = &rowStorage1;
	ComplexFloat** destRowStorage = &rowStorage2;
	ComplexFloat* chunkStorageSource = nullptr;
	ComplexFloat* chunkStorageDest = nullptr;
	unsigned int complexBufferHeight;
	unsigned int complexBufferWidth;

	void ExecuteFinalChunk(bool inverted, int length);
	void ExecuteRowOrColumn(bool inverted, int length);
	void ExecuteRowOrColumnSlow(bool inverted, int length);
	int GetReorderStorageOneStep(int index, int chunkSize);
	void ReorderStorage(int length, bool inverse = false);

	void SwapStorageBuffers();
};