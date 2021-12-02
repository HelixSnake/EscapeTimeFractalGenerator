#pragma once
#include "FractalCommandList.h"
class FractalCommandListExecutor
{
public:
	FractalCommandListExecutor(FractalCommandList commandList);
private:
	CF_Float* floats;
	int numFloats;
	ComplexFloat* cfloats;
	int numCFloats;
	
};

