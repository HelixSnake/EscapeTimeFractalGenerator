#include "MandelbrotFractal.h"

MandelbrotFractal::MandelbrotFractal()
{
	startingValue = ComplexFloat(0, 0);
	iterations = 256;
}
MandelbrotFractal::MandelbrotFractal(ComplexFloat startingValue)
{
	this->startingValue = startingValue;
	iterations = 256;
}
MandelbrotFractal::MandelbrotFractal(ComplexFloat startingValue, int iterations)
{
	this->startingValue = startingValue;
	this->iterations = iterations;
}
float MandelbrotFractal::CalculateEscapeTime(CF_Float x, CF_Float y)
{
	ComplexFloat value = startingValue;
	for (int i = 0; i < iterations; i++)
	{
		value = value * value + ComplexFloat(x, y);
		if (value.AbsoluteValue() > 4)
		{
			if (iterations != 0)
			{
				return (float)i / iterations;
			}
		}
	}
	return 0;
}