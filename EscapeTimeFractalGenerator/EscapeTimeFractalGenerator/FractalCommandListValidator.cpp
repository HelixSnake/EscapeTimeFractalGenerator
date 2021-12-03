#include "FractalCommandListValidator.h"
std::pair<FractalCommandListValidator::Error, int> FractalCommandListValidator::Check(FractalCommandList commandList, FractalCommandDelegates* delegates)
{
	if (commandList.GetCommandListLength() % 6 != 0) return { Error::CommandListLengthNotMultipleOf6, 0 };
	const unsigned int* commandArray = commandList.GetCommandList();
	int commandLength = commandList.GetCommandListLength();
	int returnValueIndex = commandLength - 6;
	for (int i = 0; i < commandLength; i += 6)
	{
		int line = i / 6 + 1;
		int function = commandArray[i];
		int returnType = commandArray[i + 1];
		int arg1Type = commandArray[i + 2];
		int arg1Index = commandArray[i + 3];
		int arg2Type = commandArray[i + 4];
		int arg2Index = commandArray[i + 5];
		if (function >= (int)FractalCommand::NUM_ITEMS) return { Error::FunctionOutOfBounds, line };
		if (returnType > 1) return {Error::ReturnTypeOutOfBounds, line };
		if (arg1Type > 3) return {Error::Arg1TypeOutOfBounds, line };
		if (arg2Type > 3) return {Error::Arg2TypeOutOfBounds, line };
		if (arg1Type < 2)
		{
			if (arg1Index < 0 || arg1Index >= commandLength) return {Error::Arg1IndexVarOutOfBounds, line };
			if (arg1Index % 6 != 0) return {Error::Arg1IndexVarNotMultipleOf6, line };
			if (arg1Index >= i && arg1Index != returnValueIndex) return {Error::Arg1IndexVarAheadButNotReturnValue, line };
			if (arg1Type != commandArray[arg1Index + 1]) return {Error::Arg1IndexVarDoesntMatchType, line };
		}
		else
		{
			if (arg1Type == 2 && arg1Index >= commandList.GetNumConstFloats()) return {Error::Arg1IndexConstOutOfBounds, line };
			if (arg1Type == 3 && arg1Index >= commandList.GetNumConstComplexFloats()) return {Error::Arg1IndexConstOutOfBounds, line };
		}
		if (arg2Type < 2)
		{
			if (arg2Index >= commandLength) return {Error::Arg2IndexVarOutOfBounds, line };
			if (arg2Index % 6 != 0) return {Error::Arg2IndexVarNotMultipleOf6, line };
			if (arg2Index >= i && arg2Index != returnValueIndex) return {Error::Arg2IndexVarAheadButNotReturnValue, line };
			if (arg2Type != commandArray[arg2Index + 1]) return {Error::Arg2IndexVarDoesntMatchType, line };
		}
		else
		{
			if (arg2Type == 2 && arg2Index >= commandList.GetNumConstFloats()) return {Error::Arg2IndexConstOutOfBounds, line };
			if (arg2Type == 3 && arg2Index >= commandList.GetNumConstComplexFloats()) return {Error::Arg2IndexConstOutOfBounds, line };
		}
		if (delegates->IsDelegatePointerNull(function, returnType, arg1Type % 2, arg2Type % 2)) return {Error::FunctionNotFound, line };
	}
	return {Error::NoError, 0 };
}

const std::unordered_map<FractalCommandListValidator::Error, std::string> FractalCommandListValidator::ErrorStrings =
{
	{
		Error::Arg1IndexConstOutOfBounds,
		"Argument 1 is const and its index is out of bounds of the const array for its type"
	},
	{
		Error::Arg1IndexVarAheadButNotReturnValue,
		"Argument 1, if a variable, must be before the current command or the index of the final command"
	},
	{
		Error::Arg1IndexVarDoesntMatchType,
		"Argument 1 is a variable and its type does not match the source command type"
	},
	{
		Error::Arg1IndexVarNotMultipleOf6,
		"Argument 1 is a variable, but the index is not a multiple of 6"
	},
	{
		Error::Arg1IndexVarOutOfBounds,
		"Argument 1 is a variable and its index is out of bounds of the command list"
	},
	{
		Error::Arg1TypeOutOfBounds,
		"Argument 1's type must be bettween 0 and 3 inclusive"
	},

	

	{
		Error::Arg2IndexConstOutOfBounds,
		"Argument 2 is const and its index is out of bounds of the const array for its type"
	},
	{
		Error::Arg2IndexVarAheadButNotReturnValue,
		"Argument 2, if a variable, must be before the current command or the index of the final command"
	},
	{
		Error::Arg2IndexVarDoesntMatchType,
		"Argument 2 is a variable and its type does not match the source command type"
	},
	{
		Error::Arg2IndexVarNotMultipleOf6,
		"Argument 2 is a variable, but the index is not a multiple of 6"
	},
	{
		Error::Arg2IndexVarOutOfBounds,
		"Argument 2 is a variable and its index is out of bounds of the command list"
	},
	{
		Error::Arg2TypeOutOfBounds,
		"Argument 2's type must be bettween 0 and 3 inclusive"
	},
	{
		Error::CommandListLengthNotMultipleOf6,
		"Command List length must be a multiple of 6"
	},
	{
		Error::FunctionNotFound,
		"Function not found"
	},
	{
		Error::FunctionOutOfBounds,
		"Function out of bounds"
	},
	{
		Error::NoError,
		"No Error"
	},
	{
		Error::ReturnTypeOutOfBounds,
		"Return type out of bounds"
	},
};