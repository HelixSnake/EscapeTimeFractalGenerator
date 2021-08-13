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
	//default algorithm for starting position
	ComplexFloat value = ComplexFloat(0, 0);
	float lengthlimitsqr = lengthLimit * lengthLimit;
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
			//default algorithm for recursive function
			value = value * value + ComplexFloat(x, y);
		}
		if (value.AbsoluteValueSqr() > lengthlimitsqr)
		{
			if (iterations != 0)
			{
				float ratio = lengthLimit / value.AbsoluteValue();
				// Adding the ratio ^ blendpower adds gradients between the iterations, smoothing out the sharp edges between colors
				return (((float)i) + pow(ratio, blendPower)) / (float)iterations;
			}
		}
	}
	return 0;
}