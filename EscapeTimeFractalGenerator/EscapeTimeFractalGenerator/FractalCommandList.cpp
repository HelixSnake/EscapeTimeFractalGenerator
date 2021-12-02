#include "FractalCommandList.h"
#include <string.h>

FractalCommandList::FractalCommandList(int numConstFloats, int numConstComplexFloats, int commandListLength, unsigned int* commandListSrc)
{
	this->numConstFloats = numConstFloats;
	this->numConstCFloats = numConstComplexFloats;
	this->commandListLength = commandListLength;
	commandList = new unsigned int[commandListLength];
	memcpy(commandList, commandListSrc, commandListLength * sizeof(unsigned int));
}
FractalCommandList::~FractalCommandList()
{
	delete[] commandList;
}

FractalCommandList::FractalCommandList(const FractalCommandList& c)
{
	this->numConstFloats = c.numConstFloats;
	this->numConstCFloats = c.numConstCFloats;
	this->commandListLength = c.commandListLength;
	commandList = new unsigned int[commandListLength];
	memcpy(commandList, c.commandList, commandListLength * sizeof(unsigned int));
}
void swap(FractalCommandList& first, FractalCommandList& second)
{
	std::swap(first.commandList, second.commandList);
	std::swap(first.commandListLength, second.commandListLength);
	std::swap(first.numConstCFloats, second.numConstCFloats);
	std::swap(first.numConstFloats, second.numConstFloats);
}