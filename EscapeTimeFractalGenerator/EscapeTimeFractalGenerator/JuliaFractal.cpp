#include "JuliaFractal.h"

JuliaFractal::JuliaFractal()
{
	startingValue = ComplexFloat(0, 0);
	iterations = 256;
}
JuliaFractal::JuliaFractal(ComplexFloat startingValue)
{
	this->startingValue = startingValue;
	iterations = 256;
}
JuliaFractal::JuliaFractal(ComplexFloat startingValue, int iterations)
{
	this->startingValue = startingValue;
	this->iterations = iterations;
}
float JuliaFractal::CalculateEscapeTime(CF_Float x, CF_Float y)
{
	ComplexFloat value = startingValue;
	for (int i = 0; i < iterations; i++)
	{
		value = value * value + ComplexFloat(x, y);
		if (value.AbsoluteValue() > 4)
		{
			if (iterations != 0)
			{
				return 1.0 - (float)i / iterations;
			}
		}
	}
	return 0;
}