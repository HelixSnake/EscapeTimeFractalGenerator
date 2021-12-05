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
	GenerateKnownCommandInputs();
}

void FractalCommandDelegates::GenerateKnownCommandDelegates() // Todo: generate all commands currently implemented
{
	CCC_Functions[(int)FractalCommand::move] = [](ComplexFloat first, ComplexFloat second) { return first; };
	FFF_Functions[(int)FractalCommand::move] = [](CF_Float first, CF_Float second) { return first; };

	CCC_Functions[(int)FractalCommand::add] = [](ComplexFloat first, ComplexFloat second) { return first + second; };
	FFF_Functions[(int)FractalCommand::add] = [](CF_Float first, CF_Float second) { return first + second; };

	CCC_Functions[(int)FractalCommand::multiply] = [](ComplexFloat first, ComplexFloat second) { return first * second; };
	FFF_Functions[(int)FractalCommand::multiply] = [](CF_Float first, CF_Float second) { return first * second; };
	CCF_Functions[(int)FractalCommand::multiply] = [](ComplexFloat first, CF_Float second) { return first * second; };
	CFC_Functions[(int)FractalCommand::multiply] = [](CF_Float first, ComplexFloat second) { return second * first; };

	CCC_Functions[(int)FractalCommand::divide] = [](ComplexFloat first, ComplexFloat second) {return first / second; };
	CCF_Functions[(int)FractalCommand::divide] = [](ComplexFloat first, CF_Float second) {return first / second; };
	FFF_Functions[(int)FractalCommand::divide] = [](CF_Float first, CF_Float second) {return first / second; };
	CFC_Functions[(int)FractalCommand::divide] = [](CF_Float first, ComplexFloat second) {return ComplexFloat::Divide(first, second); };

	CCC_Functions[(int)FractalCommand::power] = [](ComplexFloat first, ComplexFloat second) { return ComplexFloat::Power(first, second); };
	CFC_Functions[(int)FractalCommand::power] = [](CF_Float first, ComplexFloat second) { return ComplexFloat::Power(first, second); };
	FFF_Functions[(int)FractalCommand::power] = [](CF_Float first, CF_Float second) { return powl(first, second); };
	CCF_Functions[(int)FractalCommand::power] = [](ComplexFloat first, CF_Float second) { return ComplexFloat::Power(first, second); };

	CCC_Functions[(int)FractalCommand::sin] = [](ComplexFloat first, ComplexFloat second) {return ComplexFloat(sinl(first.real) * coshl(first.imaginary), cosl(first.real) * sinhl(first.imaginary)); };
	FFF_Functions[(int)FractalCommand::sin] = [](CF_Float first, CF_Float second) {return sinl(first); };

	CCC_Functions[(int)FractalCommand::cos] = [](ComplexFloat first, ComplexFloat second) {return ComplexFloat(cosl(first.real) * coshl(first.imaginary), -sinl(first.real) * sinhl(first.imaginary)); };
	FFF_Functions[(int)FractalCommand::cos] = [](CF_Float first, CF_Float second) {return cosl(first); };

	CCC_Functions[(int)FractalCommand::sinh] = [](ComplexFloat first, ComplexFloat second) {return ComplexFloat(sinhl(first.real) * cosl(first.imaginary), coshl(first.real) * sinl(first.imaginary)); };
	FFF_Functions[(int)FractalCommand::sinh] = [](CF_Float first, CF_Float second) {return sinhl(first); };

	CCC_Functions[(int)FractalCommand::cosh] = [](ComplexFloat first, ComplexFloat second) {return ComplexFloat(coshl(first.real) * cosl(first.imaginary), sinhl(first.real) * sinl(first.imaginary)); };
	FFF_Functions[(int)FractalCommand::cosh] = [](CF_Float first, CF_Float second) {return coshl(first); };

	FCC_Functions[(int)FractalCommand::magnitude] = [](ComplexFloat first, ComplexFloat second) {return first.AbsoluteValue(); };
	FCC_Functions[(int)FractalCommand::magnitudesqr] = [](ComplexFloat first, ComplexFloat second) {return first.AbsoluteValueSqr(); };
	CCC_Functions[(int)FractalCommand::normalize] = [](ComplexFloat first, ComplexFloat second) {return first.Normalized(); };
}
void FractalCommandDelegates::GenerateKnownCommandNames()
{
	for (int i = 0; i < ARRAY_LENGTH; i++)
	{
		commandNames[i] = "ERROR";
	}
	commandNames[(unsigned long long)FractalCommand::move] = "Move"; 
	commandNames[(unsigned long long)FractalCommand::add] = "Add";
	commandNames[(unsigned long long)FractalCommand::multiply] = "Multiply";
	commandNames[(unsigned long long)FractalCommand::divide] = "Divide";
	commandNames[(unsigned long long)FractalCommand::power] = "Power";
	commandNames[(unsigned long long)FractalCommand::sin] = "Sin";
	commandNames[(unsigned long long)FractalCommand::cos] = "Cos";
	commandNames[(unsigned long long)FractalCommand::sinh] = "Sinh";
	commandNames[(unsigned long long)FractalCommand::cosh] = "Cosh";
	commandNames[(unsigned long long)FractalCommand::magnitude] = "Magnitude";
	commandNames[(unsigned long long)FractalCommand::magnitudesqr] = "Magnitude Square";
	commandNames[(unsigned long long)FractalCommand::normalize] = "Normalize";
	commandNames[(unsigned long long)FractalCommand::getx] = "Get Real";
	commandNames[(unsigned long long)FractalCommand::gety] = "Get Imaginary";
	commandNames[(unsigned long long)FractalCommand::floatstocomplex] = "Build Complex Float";
}
void FractalCommandDelegates::GenerateKnownCommandInputs()
{
	for (int i = 0; i < ARRAY_LENGTH; i++)
	{
		commandInputs[i] = 2;
	}
	commandInputs[(unsigned long long)FractalCommand::move] = 1;
	commandInputs[(unsigned long long)FractalCommand::add] = 2;
	commandInputs[(unsigned long long)FractalCommand::multiply] = 2;
	commandInputs[(unsigned long long)FractalCommand::divide] = 2;
	commandInputs[(unsigned long long)FractalCommand::power] = 2;
	commandInputs[(unsigned long long)FractalCommand::sin] = 1;
	commandInputs[(unsigned long long)FractalCommand::cos] = 1;
	commandInputs[(unsigned long long)FractalCommand::sinh] = 1;
	commandInputs[(unsigned long long)FractalCommand::cosh] = 1;
	commandInputs[(unsigned long long)FractalCommand::magnitude] = 1;
	commandInputs[(unsigned long long)FractalCommand::magnitudesqr] = 1;
	commandInputs[(unsigned long long)FractalCommand::normalize] = 1;
	commandInputs[(unsigned long long)FractalCommand::getx] = 1;
	commandInputs[(unsigned long long)FractalCommand::gety] = 1;
	commandInputs[(unsigned long long)FractalCommand::floatstocomplex] = 2;
}

bool FractalCommandDelegates::IsDelegatePointerNull(int index, int returnType, int arg1Type, int arg2Type)
{
	if (returnType == 0) //Do not populate until the branches are testable!
	{
		if (arg1Type == 0)
		{
			if (arg2Type == 0) //000
			{
				return (FFF_Functions[index] == nullptr);
			}
			else //001
			{
				return (FFC_Functions[index] == nullptr);
			}
		}
		else
		{
			if (arg2Type == 0) //010
			{
				return (FCF_Functions[index] == nullptr);
			}
			else //011
			{
				return (FCC_Functions[index] == nullptr);
			}
		}
	}
	else
	{
		if (arg1Type == 0)
		{

			if (arg2Type == 0) //100
			{
				return (CFF_Functions[index] == nullptr);
			}
			else // 101
			{

				return (CFC_Functions[index] == nullptr);
			}
		}
		else
		{
			if (arg2Type == 0) //110
			{
				return (CCF_Functions[index] == nullptr);
			}
			else //111
			{
				return (CCC_Functions[index] == nullptr);
			}
		}
	}
}