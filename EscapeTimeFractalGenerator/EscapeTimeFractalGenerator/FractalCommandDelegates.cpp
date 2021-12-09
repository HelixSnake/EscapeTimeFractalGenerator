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
	GenerateKnownCommandReturnTypes();
}

void FractalCommandDelegates::GenerateKnownCommandDelegates() // Todo: generate all commands currently implemented
{

	CCC_Functions[(int)FractalCommand::move] = [](ComplexFloat first, ComplexFloat second) { return first; };
	FFF_Functions[(int)FractalCommand::move] = [](CF_Float first, CF_Float second) { return first; };

	CCC_Functions[(int)FractalCommand::add] = [](ComplexFloat first, ComplexFloat second) { return first + second; };
	FFF_Functions[(int)FractalCommand::add] = [](CF_Float first, CF_Float second) { return first + second; };
	CCF_Functions[(int)FractalCommand::add] = [](ComplexFloat first, CF_Float second) { return first + second; };
	CFC_Functions[(int)FractalCommand::add] = [](CF_Float first, ComplexFloat second) { return second + first; };

	CCC_Functions[(int)FractalCommand::subtract] = [](ComplexFloat first, ComplexFloat second) { return first - second; };
	FFF_Functions[(int)FractalCommand::subtract] = [](CF_Float first, CF_Float second) { return first - second; };
	CCF_Functions[(int)FractalCommand::subtract] = [](ComplexFloat first, CF_Float second) { return first - second; };
	CFC_Functions[(int)FractalCommand::subtract] = [](CF_Float first, ComplexFloat second) { return ComplexFloat::Subtract(first, second); };

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

	FFF_Functions[(int)FractalCommand::ln] = [](CF_Float first, CF_Float second) { return logl(first); };
	CCC_Functions[(int)FractalCommand::ln] = [](ComplexFloat first, ComplexFloat second) { return ComplexFloat::Ln(first); };

	CCC_Functions[(int)FractalCommand::sin] = [](ComplexFloat first, ComplexFloat second) {return ComplexFloat::Sin(first); };
	FFF_Functions[(int)FractalCommand::sin] = [](CF_Float first, CF_Float second) {return sinl(first); };

	CCC_Functions[(int)FractalCommand::cos] = [](ComplexFloat first, ComplexFloat second) {return ComplexFloat::Cos(first); };
	FFF_Functions[(int)FractalCommand::cos] = [](CF_Float first, CF_Float second) {return cosl(first); };

	CCC_Functions[(int)FractalCommand::tan] = [](ComplexFloat first, ComplexFloat second) {return ComplexFloat::Sin(first) / ComplexFloat::Cos(first); };
	FFF_Functions[(int)FractalCommand::tan] = [](CF_Float first, CF_Float second) {return tanl(first); };

	CCC_Functions[(int)FractalCommand::sinh] = [](ComplexFloat first, ComplexFloat second) {return ComplexFloat::Sinh(first); };
	FFF_Functions[(int)FractalCommand::sinh] = [](CF_Float first, CF_Float second) {return sinhl(first); };

	CCC_Functions[(int)FractalCommand::cosh] = [](ComplexFloat first, ComplexFloat second) {return ComplexFloat::Cosh(first); };
	FFF_Functions[(int)FractalCommand::cosh] = [](CF_Float first, CF_Float second) {return coshl(first); };

	CCC_Functions[(int)FractalCommand::tanh] = [](ComplexFloat first, ComplexFloat second) {return ComplexFloat::Sinh(first) / ComplexFloat::Cosh(first); };
	FFF_Functions[(int)FractalCommand::tanh] = [](CF_Float first, CF_Float second) {return tanhl(first); };

	//CCC_Functions[(int)FractalCommand::asin] = [](ComplexFloat first, ComplexFloat second) {return ComplexFloat::Asin(first); };
	//FFF_Functions[(int)FractalCommand::asin] = [](CF_Float first, CF_Float second) {return asinl(first); };

	FCC_Functions[(int)FractalCommand::magnitude] = [](ComplexFloat first, ComplexFloat second) {return first.AbsoluteValue(); };
	FCC_Functions[(int)FractalCommand::magnitudesqr] = [](ComplexFloat first, ComplexFloat second) {return first.AbsoluteValueSqr(); };
	FFF_Functions[(int)FractalCommand::magnitude] = [](CF_Float first, CF_Float second) {return fabsl(first); };
	FFF_Functions[(int)FractalCommand::magnitudesqr] = [](CF_Float first, CF_Float second) {return first*first; };

	CCC_Functions[(int)FractalCommand::normalize] = [](ComplexFloat first, ComplexFloat second) {return first.Normalized(); };

	FCC_Functions[(int)FractalCommand::getx] = [](ComplexFloat first, ComplexFloat second) {return first.real; };
	FCC_Functions[(int)FractalCommand::gety] = [](ComplexFloat first, ComplexFloat second) {return first.imaginary; };
	CFF_Functions[(int)FractalCommand::floatstocomplex] = [](CF_Float first, CF_Float second) {return ComplexFloat(first, second); };
	CFF_Functions[(int)FractalCommand::toimaginary] = [](CF_Float first, CF_Float second) {return ComplexFloat(0, first); };
	FCC_Functions[(int)FractalCommand::dot] = [](ComplexFloat first, ComplexFloat second) {return ComplexFloat::Dot(first, second); };
	FCC_Functions[(int)FractalCommand::arg] = [](ComplexFloat first, ComplexFloat second) {return first.Arg(); };
	FFF_Functions[(int)FractalCommand::step] = [](CF_Float first, CF_Float second) {return first >= second ? 1.0L : 0.0L; };
}
void FractalCommandDelegates::GenerateKnownCommandNames()
{
	for (int i = 0; i < ARRAY_LENGTH; i++)
	{
		commandNames[i] = "ERROR";
	}
	commandNames[(unsigned long long)FractalCommand::move] = "Move"; 
	commandNames[(unsigned long long)FractalCommand::add] = "Add";
	commandNames[(unsigned long long)FractalCommand::subtract] = "Subtract";
	commandNames[(unsigned long long)FractalCommand::multiply] = "Multiply";
	commandNames[(unsigned long long)FractalCommand::divide] = "Divide";
	commandNames[(unsigned long long)FractalCommand::power] = "Power";
	commandNames[(unsigned long long)FractalCommand::ln] = "Natural Log";
	commandNames[(unsigned long long)FractalCommand::sin] = "Sin";
	commandNames[(unsigned long long)FractalCommand::cos] = "Cos";
	commandNames[(unsigned long long)FractalCommand::tan] = "Tan";
	commandNames[(unsigned long long)FractalCommand::sinh] = "Sinh";
	commandNames[(unsigned long long)FractalCommand::cosh] = "Cosh";
	commandNames[(unsigned long long)FractalCommand::tanh] = "Tanh";
	//commandNames[(unsigned long long)FractalCommand::asin] = "Arcsin";
	commandNames[(unsigned long long)FractalCommand::magnitude] = "Magnitude";
	commandNames[(unsigned long long)FractalCommand::magnitudesqr] = "Magnitude Square";
	commandNames[(unsigned long long)FractalCommand::normalize] = "Normalize";
	commandNames[(unsigned long long)FractalCommand::getx] = "Get Real";
	commandNames[(unsigned long long)FractalCommand::gety] = "Get Imaginary";
	commandNames[(unsigned long long)FractalCommand::floatstocomplex] = "Build Complex Float";
	commandNames[(unsigned long long)FractalCommand::toimaginary] = "To Imaginary";
	commandNames[(unsigned long long)FractalCommand::dot] = "Dot";
	commandNames[(unsigned long long)FractalCommand::arg] = "Arg";
	commandNames[(unsigned long long)FractalCommand::step] = "Step";
}
void FractalCommandDelegates::GenerateKnownCommandInputs()
{
	for (int i = 0; i < ARRAY_LENGTH; i++)
	{
		commandInputs[i] = 2;
	}
	commandInputs[(unsigned long long)FractalCommand::move] = 1;
	commandInputs[(unsigned long long)FractalCommand::add] = 2;
	commandInputs[(unsigned long long)FractalCommand::subtract] = 2;
	commandInputs[(unsigned long long)FractalCommand::multiply] = 2;
	commandInputs[(unsigned long long)FractalCommand::divide] = 2;
	commandInputs[(unsigned long long)FractalCommand::power] = 2;
	commandInputs[(unsigned long long)FractalCommand::ln] = 1;
	commandInputs[(unsigned long long)FractalCommand::sin] = 1;
	commandInputs[(unsigned long long)FractalCommand::cos] = 1;
	commandInputs[(unsigned long long)FractalCommand::tan] = 1;
	commandInputs[(unsigned long long)FractalCommand::sinh] = 1;
	commandInputs[(unsigned long long)FractalCommand::cosh] = 1;
	commandInputs[(unsigned long long)FractalCommand::tanh] = 1;
	//commandInputs[(unsigned long long)FractalCommand::asin] = 1;
	commandInputs[(unsigned long long)FractalCommand::magnitude] = 1;
	commandInputs[(unsigned long long)FractalCommand::magnitudesqr] = 1;
	commandInputs[(unsigned long long)FractalCommand::normalize] = 1;
	commandInputs[(unsigned long long)FractalCommand::getx] = 1;
	commandInputs[(unsigned long long)FractalCommand::gety] = 1;
	commandInputs[(unsigned long long)FractalCommand::floatstocomplex] = 2;
	commandInputs[(unsigned long long)FractalCommand::toimaginary] = 1;
	commandInputs[(unsigned long long)FractalCommand::dot] = 2;
	commandInputs[(unsigned long long)FractalCommand::arg] = 1;
	commandInputs[(unsigned long long)FractalCommand::step] = 2;
}

void FractalCommandDelegates::GenerateKnownCommandReturnTypes()
{

	for (int i = 0; i < ARRAY_LENGTH; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				resultTypes[i][j][k] = -1;
			}
		}
	}
	for (int i = 0; i < ARRAY_LENGTH; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				for (int l = 0; l < 2; l++)
				{
					if (!IsDelegatePointerNull(i, l, j, k))
					{
						resultTypes[i][j][k] = l;
						continue;
					}
				}
			}
		}
	}
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