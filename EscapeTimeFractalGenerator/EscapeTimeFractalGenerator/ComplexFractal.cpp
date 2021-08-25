#include "ComplexFractal.h"
#include "math.h"
#include "glm/common.hpp"

ComplexFractal::ComplexFractal()
{
	iterations = 256;
}
ComplexFractal::ComplexFractal(int iterations)
{
	this->iterations = iterations;
}
ComplexFractal::ComplexFractal(int iterations, CF_Float minDeviation)
{
	this->iterations = iterations;
	this->minDeviationSqr = minDeviation * minDeviation;
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
	// adding this constant to the value will insure that the fractal does not escape the first iteration due to the minimum deviation
	ComplexFloat prevValue = value + ComplexFloat(minDeviationSqr, minDeviationSqr);
	float lengthLimitSqr = lengthLimit * lengthLimit;
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
		float absValSqr = value.AbsoluteValueSqr();
		if (absValSqr > lengthLimitSqr)
		{
			float ratio = (lengthLimit) / value.AbsoluteValue();
			ratio = glm::clamp(ratio, 0.0f, 1.0f); // Extra insurance to keep the function below from spitting out NAN if ratio < 0 
			//or infinity if lengthLimit == 1
			const double MAGIC_CONSTANT = 1.581983; // I have no idea what this number is; it's very close to 1/(1-1/e) or 1.58197670687
			//but if I use that number you get results that are slightly off. Just roll with this for now.
			//please for the love of God do not ask me why this works but it's necessary to make the gradient appear linear
			ratio = pow(ratio, 1/log(lengthLimit))* MAGIC_CONSTANT;
			return (float)i + ratio;
			//return (float)i;
		}
		ComplexFloat deviation = value - prevValue;
		if (deviation.AbsoluteValueSqr() < minDeviationSqr)
		{
			return 0;
		}
		prevValue = value;
	}
	return 0;
}