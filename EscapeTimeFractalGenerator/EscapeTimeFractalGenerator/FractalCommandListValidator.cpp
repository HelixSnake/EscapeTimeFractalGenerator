#include "FractalCommandListValidator.h"
FractalCommandListValidator::Error FractalCommandListValidator::Check(FractalCommandList commandList, FractalCommandDelegates* delegates)
{
	if (delegates == nullptr) return Error::DelegatesNullPointer;
	if (commandList.GetCommandListLength() % 6 != 0) return Error::CommandListLengthNotMultipleOf6;
	const unsigned int* commandArray = commandList.GetCommandList();
	int commandLength = commandList.GetCommandListLength();
	int returnValueIndex = commandLength - 6;
	for (int i = 0; i < commandLength; i += 6)
	{
		int function = commandArray[i];
		int returnType = commandArray[i + 1];
		int arg1Type = commandArray[i + 2];
		int arg1Index = commandArray[i + 3];
		int arg2Type = commandArray[i + 4];
		int arg2Index = commandArray[i + 5];
		if (function < 0 || function >= (int)FractalCommand::NUM_ITEMS) return Error::FunctionOutOfBounds;
		if (returnType < 0 || returnType > 1) return Error::ReturnTypeOutOfBounds;
		if (arg1Type < 0 || arg1Type > 3) return Error::Arg1TypeOutOfBounds;
		if (arg2Type < 0 || arg2Type > 3) return Error::Arg2TypeOutOfBounds;
		if (arg1Type < 2)
		{
			if (arg1Index < 0 || arg1Index >= commandLength) return Error::Arg1IndexVarOutOfBounds;
			if (arg1Index % 6 != 0) return Error::Arg1IndexVarNotMultipleOf6;
			if (arg1Index == i) return Error::Arg1IndexVarPointsToCurrentCommand;
			if (arg1Index > i && arg1Index != returnValueIndex) return Error::Arg1IndexVarAheadButNotReturnValue;
			if (arg1Type != commandArray[arg1Index + 1]) return Error::Arg1IndexVarDoesntMatchType;
		}
		else
		{
			if (arg1Index < 0) return Error::Arg1IndexConstOutOfBounds;
			if (arg1Type == 2 && arg1Index >= commandList.GetNumConstFloats()) return Error::Arg1IndexConstOutOfBounds;
			if (arg1Type == 3 && arg1Index >= commandList.GetNumConstComplexFloats()) return Error::Arg1IndexConstOutOfBounds;
		}


		if (arg2Type < 2)
		{
			if (arg2Index < 0 || arg2Index >= commandLength) return Error::Arg2IndexVarOutOfBounds;
			if (arg2Index % 6 != 0) return Error::Arg2IndexVarNotMultipleOf6;
			if (arg2Index == i) return Error::Arg2IndexVarPointsToCurrentCommand;
			if (arg2Index > i && arg2Index != returnValueIndex) return Error::Arg2IndexVarAheadButNotReturnValue;
			if (arg2Type != commandArray[arg2Index + 1]) return Error::Arg2IndexVarDoesntMatchType;
		}
		else
		{
			if (arg2Index < 0) return Error::Arg2IndexConstOutOfBounds;
			if (arg2Type == 2 && arg2Index >= commandList.GetNumConstFloats()) return Error::Arg2IndexConstOutOfBounds;
			if (arg2Type == 3 && arg2Index >= commandList.GetNumConstComplexFloats()) return Error::Arg2IndexConstOutOfBounds;
		}
	}
	return Error::NoError;
}