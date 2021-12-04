#pragma once
#include "FractalCommandList.h"
#include "FractalCommandDelegates.h"
#include <unordered_map>
#include <string>
class FractalCommandListValidator
{
public:
	enum class Error
	{
		NoError,
		CommandListEmpty,
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
		Arg1IndexVarDoesntMatchType,
		Arg2IndexVarDoesntMatchType,
		Arg1IndexConstOutOfBounds,
		Arg2IndexConstOutOfBounds,
		NUM_ITEMS
	};

	static const std::unordered_map<Error, std::string> ErrorStrings;

	static std::pair<Error, int>  Check(FractalCommandList commandList, FractalCommandDelegates *delegates);
};

