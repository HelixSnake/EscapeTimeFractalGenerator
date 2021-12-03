#include "FractalCommandDelegates.h"
#include <string>



FractalCommandDelegates::FractalCommandDelegates()
{
	memset(FFF_Functions, 0, ARRAY_LENGTH * sizeof(FloatFloatFloat));
	memset(FFC_Functions, 0, ARRAY_LENGTH * sizeof(FloatFloatCF));
	memset(FCF_Functions, 0, ARRAY_LENGTH * sizeof(FloatCFFloat));
	memset(FCC_Functions, 0, ARRAY_LENGTH * sizeof(FloatCFCF));
	memset(CFF_Functions, 0, ARRAY_LENGTH * sizeof(CFFloatFloat));
	memset(CFC_Functions, 0, ARRAY_LENGTH * sizeof(CFFloatCF));
	memset(CCF_Functions, 0, ARRAY_LENGTH * sizeof(CFCFFloat));
	memset(CCC_Functions, 0, ARRAY_LENGTH * sizeof(CFCFCF));
	GenerateKnownCommandDelegates(); 
	GenerateKnownCommandNames();
}

void FractalCommandDelegates::GenerateKnownCommandDelegates() // Todo: generate all commands currently implemented
{
	CCC_Functions[(int)FractalCommand::move] = [](ComplexFloat first, ComplexFloat second) { return first; };
	CCC_Functions[(int)FractalCommand::add] = [](ComplexFloat first, ComplexFloat second) { return first + second; };
	CCC_Functions[(int)FractalCommand::multiply] = [](ComplexFloat first, ComplexFloat second) { return first * second; };
}
void FractalCommandDelegates::GenerateKnownCommandNames() // Todo: generate all commands currently implemented
{
	for (int i = 0; i < ARRAY_LENGTH; i++)
	{
		commandNames[i] = "ERROR";
	}
	commandNames[(unsigned long long)FractalCommand::move] = "Move"; 
	commandNames[(unsigned long long)FractalCommand::add] = "Add";
	commandNames[(unsigned long long)FractalCommand::multiply] = "Multiply";
	commandNames[(unsigned long long)FractalCommand::power] = "Power";
	commandNames[(unsigned long long)FractalCommand::sin] = "Sin";
	commandNames[(unsigned long long)FractalCommand::cos] = "Cos";
	commandNames[(unsigned long long)FractalCommand::magnitude] = "Magnitude";
	commandNames[(unsigned long long)FractalCommand::normalize] = "Normalize";
	commandNames[(unsigned long long)FractalCommand::getx] = "Get Real";
	commandNames[(unsigned long long)FractalCommand::gety] = "Get Imaginary";
	commandNames[(unsigned long long)FractalCommand::floatstocomplex] = "Build Complex Float";
}

bool FractalCommandDelegates::IsDelegatePointerNull(int index, int returnType, int arg1Type, int arg2Type)
{
	if (returnType == 0) //Do not populate until the branches are testable!
	{
		if (arg1Type == 0)
		{
			if (arg2Type == 0) //000
			{
				return true;
			}
			else //001
			{

				return true;
			}
		}
		else
		{
			if (arg2Type == 0) //010
			{

				return true;
			}
			else //011
			{

				return true;
			}
		}
	}
	else
	{
		if (arg1Type == 0)
		{

			if (arg2Type == 0) //100
			{

				return true;
			}
			else // 101
			{

				return true;
			}
		}
		else
		{
			if (arg2Type == 0) //110
			{
				return true;
			}
			else //111
			{
				return (CCC_Functions[index] == nullptr);
			}
		}
	}
}