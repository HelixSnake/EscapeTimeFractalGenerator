#pragma once
#include "ComplexFloat.h"
class FractalCommandTree
{
public:
	enum Commands
	{
		add,
		multiply,
		power,
		sin,
		cos,
		magnitude,
		normalize,
		getx,
		gety
	};
	FractalCommandTree(int numInts, int numFloats, int numComplexFloats);
	~FractalCommandTree();
private:
	int* intsources = nullptr;
	int numIntSources = 0;
	CF_Float* floatsources = nullptr;
	int numFloatSources = 0;
	ComplexFloat* cfSources = nullptr;
	int numCfSources = 0;

	int* intsStored = nullptr;
	int numIntsStored = 0;
	CF_Float* floatsStored = nullptr;
	int numFloatsStored = 0;
	ComplexFloat* cfStored = nullptr;
	int numCfStored = 0;

	int* commandTree = nullptr;
	int commandTreeLength = 0; //the total length of the command tree
};
//Command tree layout:
//each command is 5 blocks wide
//stored as the following
//[command enum] [argument 1 type] [argument 1 index] [argument 2 type] [argument 2 index]
//types:
//
//source ints: 0
//source floats: 1
//source complex floats: 2
// 
//stored ints: 3
//stored floats: 4
//stored complex floats: 5 

//previous value: 6
//input value: 7
//julia value: 8
//
//for previous value (6), input value (7), or julia value (8), the index will be ignored.
//
