#pragma once
#include "ComplexFloat.h"
class FractalCommandList //Readonly Data storage only, do not implement any functionality here besides construction and reading. This data stored in this class is stored in a way meant to be executed upon as quickly as possible.
//Todo: Add unit tests
//Todo: Builder class
//Todo: Execution class
//Todo: Validator class (if the command list has bad data, the execution class may crash the program!)
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
		NUM_ENTRIES
	};
	FractalCommandList(int numFloats, int numComplexFloats, int commandListLength, unsigned int* commandListSrc);
	~FractalCommandList();
	int GetNumFloats() { return numFloats; }
	int GetNumComplexFloats() { return numComplexFloats; }
	int GetCommandListLength() { return commandListLength; }
	const unsigned int* GetCommandList() {	return commandList;	}
	static int RunTests();
private:
	unsigned int* commandList = nullptr;
	int commandListLength = 0; //the total length of the command tree in indices
	int numFloats = 0;
	int numComplexFloats = 0;
};
//Command list layout:
//each command is 7 blocks wide
//stored as the following
//[command enum] [command type] [command index] [argument 1 type] [argument 1 index] [argument 2 type] [argument 2 index]

//Data is stored in 6 arrays (not in this class), the argument types and command types will refer to these arrays
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