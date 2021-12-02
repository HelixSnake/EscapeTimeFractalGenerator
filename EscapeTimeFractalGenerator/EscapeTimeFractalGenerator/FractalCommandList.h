#pragma once
#include "ComplexFloat.h"
#include <algorithm>
class FractalCommandList //Readonly Data storage only, do not implement any functionality here besides construction and reading. This data stored in this class is stored in a way meant to be executed upon as quickly as possible.
//You should only need one instance of this class at a time in the program. A new one should be created every time you need to change it.
//Todo: Execution class
//Todo: Builder class
//Todo: Validator class (if the command list has bad data, the execution class may crash the program!)
//Todo: Parser (parse strings into Builder class)
{
public:
	enum struct Command
	{
		add,
		multiply,
		power,
		sin,
		cos,
		magnitude,
		normalize,
		getx,
		gety,
		floatstocomplex,
		ALL_NULL, //used for testing; no delegates will exist for this command
		NUM_ITEMS
	};
	FractalCommandList(int numConstFloats, int numConstCFloats, int commandListLength, unsigned int* commandListSrc);
	FractalCommandList(const FractalCommandList& c);
	~FractalCommandList();
	FractalCommandList& operator=(FractalCommandList other)
	{
		using std::swap;
		swap(*this, other);
		return *this;
	}
	friend void swap(FractalCommandList&, FractalCommandList&);
	int GetNumConstFloats() { return numConstFloats; }
	int GetNumConstComplexFloats() { return numConstCFloats; }
	int GetCommandListLength() { return commandListLength; }
	int GetResultIndex() { return commandListLength - 6; } // The result index, or the index of the return value of the command list, will always be the index of the start of the final command block
	const unsigned int* GetCommandList() {	return commandList;	}
private:
	unsigned int* commandList = nullptr;
	int commandListLength = 0; //the total length of the command tree in indices
	int numConstFloats = 0;
	int numConstCFloats = 0;
};
//Command list layout:
//each command is 6 blocks wide
//stored as the following
//[command enum] [command type] [argument 1 type] [argument 1 index] [argument 2 type] [argument 2 index]

//Data is stored in 4 arrays (not in this class), the argument types and command types will refer to these arrays
//variable floats: 0
//variable complex floats: 1
// 
//constant floats: 2
//constant complex floats: 3
//
// [command type] will be 0, or 1, corresponding to these arrays. [argument type] can be any of the four numbers.
// if a command is only has one input, [argument 2 type] and [argument 2 index] will be ignored and set to 0.
// Data for variables (non constants) will be stored in arrays with the same length as the command tree, with data only stored and used once every 7 entries. Wasted space, but it's faster and easier to keep track of, and they shouldn't be that long anyways.
// Every time a command is executed, the result will be stored in same spot at the beginning of that command chunk, in the array of variables corresponding to the command type.
// Argument indexes should either be 1. the position of a previous command, and the same type as that command type was, or 2. the index of a constant in one of the constants arrays, if the type points to a constant array.