#pragma once
#include "FractalCommandList.h"
#include "FractalCommandDelegates.h"
class FractalCommandListValidator
{
public:
	enum class Error
	{
		NoError,
		DelegatesNullPointer,
		CommandListLengthNotMultipleOf6,
		FunctionOutOfBounds,
		ReturnTypeOutOfBounds,
		Arg1TypeOutOfBounds,
		Arg2TypeOutOfBounds,
		FunctionNotFound,
		Arg1IndexVarOutOfBounds,
		Arg2IndexVarOutOfBounds,
		Arg1IndexVarNotMultipleOf6,
		Arg2IndexVarNotMultipleOf6,
		Arg1IndexVarAheadButNotReturnValue,
		Arg2IndexVarAheadButNotReturnValue,
		Arg1IndexVarPointsToCurrentCommand,
		Arg2IndexVarPointsToCurrentCommand,
		Arg1IndexVarDoesntMatchType,
		Arg2IndexVarDoesntMatchType,
		Arg1IndexConstOutOfBounds,
		Arg2IndexConstOutOfBounds
	};
	static Error Check(FractalCommandList commandList, FractalCommandDelegates *delegates);
};

