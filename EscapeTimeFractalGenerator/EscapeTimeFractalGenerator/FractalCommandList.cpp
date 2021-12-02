#include "FractalCommandList.h"
#include <string.h>

FractalCommandList::FractalCommandList(int numFloats, int numComplexFloats, int commandListLength, unsigned int* commandListSrc)
{
	this->numFloats = numFloats;
	this->numComplexFloats = numComplexFloats;
	this->commandListLength = commandListLength;
	commandList = new unsigned int[commandListLength];
	memcpy(commandList, commandListSrc, commandListLength * sizeof(unsigned int));
}
FractalCommandList::~FractalCommandList()
{
	if (commandList != nullptr)
	{
		delete[] commandList;
	}
}