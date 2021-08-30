#include "ComplexFractal.h"
#include "math.h"
#include "glm/common.hpp"

const float PI = 3.14159265359;

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
void ComplexFractal::SetFunction(ComplexFloat(*func)(ComplexFloat input, ComplexFloat previousValue, ComplexFloat extraValue))
{
	ComplexFunction = func;
}
void ComplexFractal::SetStartingFunction(ComplexFloat(*func)(ComplexFloat input, ComplexFloat extraValue))
{
	StartingValueFunction = func;
}
double ComplexFractal::CalculateEscapeTime(CF_Float x, CF_Float y, ComplexFloat extraValue, glm::vec2 &UV)
{
	//default algorithm for starting position
	ComplexFloat value = ComplexFloat(0, 0);
	double lengthlimitsqr = lengthLimit * lengthLimit;
	if (StartingValueFunction != nullptr)
	{ 
		value = StartingValueFunction(ComplexFloat(x, y), extraValue);
	}
	// adding this constant to the value will insure that the fractal does not escape the first iteration due to the minimum deviation
	ComplexFloat prevValue = value + ComplexFloat(minDeviationSqr, minDeviationSqr);
	ComplexFloat prev2Value = value + ComplexFloat(minDeviationSqr, minDeviationSqr);
	double lengthLimitSqr = lengthLimit * lengthLimit;
	for (int i = 0; i < iterations; i++)
	{
		if (ComplexFunction != nullptr)
		{
			value = ComplexFunction(ComplexFloat(x, y), value, extraValue);
		}
		else
		{
			//default algorithm for recursive function
			value = value * value + ComplexFloat(x, y);
		}
		double absValSqr = value.AbsoluteValueSqr();
		if (absValSqr > lengthLimitSqr)
		{
			double ratio = (lengthLimit) / value.AbsoluteValue();
			ratio = glm::clamp(ratio, 0.0, 1.0); // Extra insurance to keep the function below from spitting out NAN if ratio < 0 
			//or infinity if lengthLimit == 1
			const double MAGIC_CONSTANT = 1.581983; // I have no idea what this number is; it's very close to 1/(1-1/e) or 1.58197670687
			//but if I use that number you get results that are slightly off. Just roll with this for now.
			//please for the love of God do not ask me why this works but it's necessary to make the gradient appear linear
			ratio = pow(ratio, 1/log(lengthLimit)) * MAGIC_CONSTANT + 1 - MAGIC_CONSTANT;

			double angle = ComplexFloat::Angle(value - prevValue, prevValue - prev2Value);
			angle = angle / PI / 2.0 + 0.5;
			angle = glm::fract(angle * 2); // for a better aspect ratio
			UV = glm::vec2(angle, 1-ratio);
			return (double)i + ratio;
			//return (float)i;
		}
		ComplexFloat deviation = value - prevValue;
		if (deviation.AbsoluteValueSqr() < minDeviationSqr)
		{
			return 0;
		}
		prev2Value = prevValue;
		prevValue = value;
	}
	return 0;
}