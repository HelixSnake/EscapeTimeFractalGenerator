#pragma once
#include "FractalCommandList.h"
#include "FractalCommandDelegates.h"
class FractalCommandListExecutor //Executes the fractal command list as many times as necessary. You should only need one of these per thread.
{
public:
	FractalCommandListExecutor(FractalCommandList commandList, FractalCommandDelegates* delegates);
	bool SetConstantFloat(int index, CF_Float value); //returns false if failed
	bool SetConstantComplexFloat(int index, ComplexFloat value); //returns false if failed
	int Execute(); // returns 0 if succeeded, 1 if a function is unknown
	void InitializeReturnValue(ComplexFloat value);
	ComplexFloat GetReturnValue();
	~FractalCommandListExecutor();
private:
	FractalCommandList commandList;
	FractalCommandDelegates* delegates;
	CF_Float* floats;
	int numFloats;
	ComplexFloat* cfloats;
	int numCFloats;
	CF_Float* constFloats;
	int numConstFloats;
	ComplexFloat* constCFloats;
	int numConstCFloats;
	const void* arraysByNumber[4];
};

