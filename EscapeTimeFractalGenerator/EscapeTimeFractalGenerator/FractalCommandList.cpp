#include "FractalCommandList.h"
#include <string>
#include <iostream>

FractalCommandList::FractalCommandList(int numConstFloats, int numConstComplexFloats, int commandListLength, const unsigned int* commandListSrc)
{
	this->numConstFloats = numConstFloats;
	this->numConstCFloats = numConstComplexFloats;
	this->commandListLength = commandListLength;
	commandList = new unsigned int[commandListLength];
	if (commandListSrc != nullptr)
	{
		memcpy(commandList, commandListSrc, commandListLength * sizeof(unsigned int));
	}
}

FractalCommandList::FractalCommandList(int numConstFloats, int numConstComplexFloats, std::vector<unsigned int> commandListVector)
{
	this->numConstFloats = numConstFloats;
	this->numConstCFloats = numConstComplexFloats;
	this->commandListLength = commandListVector.size();
	this->commandList = new unsigned int[commandListLength];
	for (int i = 0; i < commandListLength; i++) //memcpy COULD work but I don't trust it in this instance.
	{
		commandList[i] = commandListVector[i];
	}
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

std::string FractalCommandList::ToString()
{
	std::string output = "Num constant floats: " + std::to_string(this->numConstFloats) + ", Num constant complex floats:" + std::to_string(this->numConstCFloats);
	for (int i = 0; i < commandListLength; i++)
	{
		if (i % 6 == 0) output = output + +"\n";
		output = output + std::to_string(commandList[i]) + ", ";
	}
	return output;
}