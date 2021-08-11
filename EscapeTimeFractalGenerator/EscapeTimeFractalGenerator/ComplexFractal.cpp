#include "ComplexFractal.h"
#include "math.h"

ComplexFractal::ComplexFractal()
{
	iterations = 256;
}
ComplexFractal::ComplexFractal(int iterations)
{
	this->iterations = iterations;
}
void ComplexFractal::SetFunction(ComplexFloat(*func)(ComplexFloat input, ComplexFloat previousValue, float time))
{
	ComplexFunction = func;
}
void ComplexFractal::SetStartingFunction(ComplexFloat(*func)(ComplexFloat input, float time))
{
	StartingValueFunction = func;
}
float ComplexFractal::CalculateEscapeTime(CF_Float x, CF_Float y, float time)
{
	ComplexFloat value = ComplexFloat(0, 0);
	if (StartingValueFunction != nullptr)
	{ 
		value = StartingValueFunction(ComplexFloat(x, y), time);
	}
	for (int i = 0; i < iterations; i++)
	{
		if (ComplexFunction != nullptr)
		{
			value = ComplexFunction(ComplexFloat(x, y), value, time);
		}
		else
		{
			//default algorithm
			value = value * value + ComplexFloat(x, y);
		}
		float ratio = lengthLimit / value.AbsoluteValue();
		if (ratio < 1)
		{
			if (iterations != 0)
			{
				return (((float)i) + pow(ratio, blendPower)) / (float)iterations;
			}
		}
	}
	return 0;
}