#include "FractalCommandListExecutor.h"

FractalCommandListExecutor::FractalCommandListExecutor(FractalCommandList commandList, FractalCommandDelegates* delegates)
	:commandList(commandList)
{
	this->commandList = commandList;
	int commandListLength = commandList.GetCommandListLength();
	floats = new CF_Float[commandListLength];
	numFloats = commandListLength;
	cfloats = new ComplexFloat[commandListLength];
	numCFloats = commandListLength;
	numConstFloats = commandList.GetNumConstFloats();
	constFloats = new CF_Float[numConstFloats];
	numConstCFloats = commandList.GetNumConstComplexFloats();
	constCFloats = new ComplexFloat[numConstCFloats];
	this->delegates = delegates;
	arraysByNumber[0] = floats;
	arraysByNumber[1] = cfloats;
	arraysByNumber[2] = constFloats;
	arraysByNumber[3] = constCFloats;
};

FractalCommandListExecutor::~FractalCommandListExecutor()
{
	delete[] floats;
	delete[] cfloats;
	delete[] constFloats;
	delete[] constCFloats;
}
bool FractalCommandListExecutor::SetConstantFloat(int index, CF_Float value)
{
	if (index < 0 || index >= numConstFloats) return false;
	constFloats[index] = value;
}
bool FractalCommandListExecutor::SetConstantComplexFloat(int index, ComplexFloat value)
{
	if (index < 0 || index >= numConstCFloats) return false;
	constCFloats[index] = value;
}
int FractalCommandListExecutor::Execute()
{
	const unsigned int* commandListArray = commandList.GetCommandList();
	int commandListLength = commandList.GetCommandListLength();
	// Sorry this is such a mess, this is the fastest I could make the code without making the function pointers type unsafe. Just roll with it.
	for (int i = 0; i < commandListLength; i += 6)
	{
		int commandType = commandListArray[i + 1];
		int arg1Src = commandListArray[i + 2];
		int arg1Index = commandListArray[i + 3];
		int arg2Src = commandListArray[i + 4];
		int arg2Index = commandListArray[i + 5];
		int arg1Type = arg1Src % 2; // 0 if float, 1 if complex float
		int arg2Type = arg2Src % 2; // 0 if float, 1 if complex float
		//DON'T IMPLEMENT ANY LINE OF THIS WITHOUT A FUNCTION TO TEST IT WITH AND A TEST WRITTEN
		//IF YOU DO NOT HAVE A FUNCTION WITH THE RIGHT COMBINATION LEAVE IT AS "return 1;"
		if (commandType == 0)
		{
			if (arg1Type == 0)
			{
				if (arg2Type == 0) //000
				{
					return 1;
				}
				else //001
				{

					return 1;
				}
			}
			else
			{
				if (arg2Type == 0) //010
				{

					return 1;
				}
				else //011
				{

					return 1;
				}
			}
		}
		else
		{
			if (arg1Type == 0)
			{

				if (arg2Type == 0) //100
				{

					return 1;
				}
				else // 101
				{

					return 1;
				}
			}
			else
			{
				if (arg2Type == 0) //110
				{
					return 1;
				}
				else //111
				{
					if (delegates->CCC_Functions[commandListArray[i]] == nullptr) return 1;
					cfloats[i] = delegates->CCC_Functions[commandListArray[i]](((ComplexFloat*)arraysByNumber[arg1Src])[arg1Index], ((ComplexFloat*)arraysByNumber[arg2Src])[arg2Index]);
				}
			}
		}
	}
	return 0;
}
void FractalCommandListExecutor::InitializeReturnValue(ComplexFloat value)
{
	cfloats[commandList.GetResultIndex()] = value;
}
ComplexFloat FractalCommandListExecutor::GetReturnValue()
{
	return cfloats[commandList.GetResultIndex()];
}