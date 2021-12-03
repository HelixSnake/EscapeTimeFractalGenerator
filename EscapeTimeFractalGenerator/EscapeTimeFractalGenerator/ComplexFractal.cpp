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
ComplexFractal::ComplexFractal(int iterations, CF_Float minDeviation, int deviationCycle, bool debugDeviations)
{
	this->iterations = iterations;
	this->minDeviationSqr = minDeviation * minDeviation;
	this->deviationCycle = deviationCycle;
	this->debugDeviations = debugDeviations;
}
void ComplexFractal::SetFunction(RecursiveFunction func)
{
	recursiveFunction = func;
}
void ComplexFractal::SetStartingFunction(StartingValueFunction func)
{
	startingValueFunction = func;
}
double ComplexFractal::CalculateEscapeTime(CF_Float x, CF_Float y, ComplexFloat* extraValues, int power)
{
	//default algorithm for starting position
	ComplexFloat value = ComplexFloat(0, 0);
	double lengthlimitsqr = lengthLimit * lengthLimit;
	if (startingValueFunction != nullptr)
	{ 
		value = startingValueFunction(ComplexFloat(x, y), extraValues, power);
	}
	// adding this constant to the value will insure that the fractal does not escape the first iteration due to the minimum deviation
	ComplexFloat prevValue = value + ComplexFloat(minDeviationSqr, minDeviationSqr);
	ComplexFloat cycleValue = value + ComplexFloat(minDeviationSqr, minDeviationSqr);
	ComplexFloat prev2Value = value + ComplexFloat(minDeviationSqr, minDeviationSqr);
	double lengthLimitSqr = lengthLimit * lengthLimit;
	for (int i = 0; i < iterations; i++)
	{
		if (recursiveFunction != nullptr)
		{
			value = recursiveFunction(ComplexFloat(x, y), value, extraValues, power);
		}
		else
		{
			//default algorithm for recursive function
			value = value * value + ComplexFloat(x, y);
		}
		if (isnan(value.real) || isnan(value.imaginary)) return i; // if the values have gotten so extreme that a NAN showed up, just assume we diverged. Trust me, this is a good idea.
		double absValSqr = value.AbsoluteValueSqr();
		if (absValSqr > lengthLimitSqr)
		{
			double ratio = (lengthLimit) / value.AbsoluteValue();
			ratio = glm::clamp(ratio, 0.0, 1.0); // Extra insurance to keep the function below from spitting out NAN if ratio < 0 
			//or infinity if lengthLimit == 1
			//TODO: This algorithm only works at a power of 2. Figure out the proper math for other powers
			//I could not find the algorithm for the other powers so the following matrix is composed of hand-tuned values
			//More values have to be tuned in order to support powers over 7
			const double part1[6] = { 2, 1.86, 1.74, 1.64, 1.55, 1.47 };
			const double part2[6] = { 2, 1.406576, 1.234300, 1.160412, 1.125840, 1.110008 };
			int index = glm::clamp(power - 2, 0, 5); // prevent out of bounds operations 
			ratio = pow(ratio, log(part1[index])/(log(lengthLimit))) * part2[index];
			//double angle = ComplexFloat::Angle(value - prevValue, prevValue - prev2Value);
			//angle = angle / PI / 2.0 + 0.5;
			//angle = glm::fract(angle * 2); // for a better aspect ratio
			//UV = glm::vec2(angle, 1-ratio);
			return (double)i + ratio;
			//return (float)i;
		}
		ComplexFloat deviation = value - cycleValue;
		if (deviation.AbsoluteValueSqr() < minDeviationSqr)
		{
			return debugDeviations? i : 0;
		}
		prev2Value = prevValue;
		prevValue = value;
		if (i % deviationCycle == 0) cycleValue = value;
	}
	return 0;
}


double ComplexFractal::CalculateEscapeTime(FractalCommandListExecutor& startingFunction, FractalCommandListExecutor& recursiveFunction, int power)
{//default algorithm for starting position
	ComplexFloat value = ComplexFloat(0, 0);
	double lengthlimitsqr = lengthLimit * lengthLimit;
	startingFunction.Execute();
	value = startingFunction.GetReturnValue();
	// adding this constant to the value will insure that the fractal does not escape the first iteration due to the minimum deviation
	ComplexFloat prevValue = value + ComplexFloat(minDeviationSqr, minDeviationSqr);
	ComplexFloat cycleValue = value + ComplexFloat(minDeviationSqr, minDeviationSqr);
	ComplexFloat prev2Value = value + ComplexFloat(minDeviationSqr, minDeviationSqr);
	double lengthLimitSqr = lengthLimit * lengthLimit;
	for (int i = 0; i < iterations; i++)
	{
		recursiveFunction.InitializeReturnValue(value);
		recursiveFunction.Execute();
		value = recursiveFunction.GetReturnValue();
		if (isnan(value.real) || isnan(value.imaginary)) return i; // if the values have gotten so extreme that a NAN showed up, just assume we diverged. Trust me, this is a good idea.
		double absValSqr = value.AbsoluteValueSqr();
		if (absValSqr > lengthLimitSqr)
		{
			double ratio = (lengthLimit) / value.AbsoluteValue();
			ratio = glm::clamp(ratio, 0.0, 1.0); // Extra insurance to keep the function below from spitting out NAN if ratio < 0 
			//or infinity if lengthLimit == 1
			//TODO: This algorithm only works at a power of 2. Figure out the proper math for other powers
			//I could not find the algorithm for the other powers so the following matrix is composed of hand-tuned values
			//More values have to be tuned in order to support powers over 7
			const double part1[6] = { 2, 1.86, 1.74, 1.64, 1.55, 1.47 };
			const double part2[6] = { 2, 1.406576, 1.234300, 1.160412, 1.125840, 1.110008 };
			int index = glm::clamp(power - 2, 0, 5); // prevent out of bounds operations 
			ratio = pow(ratio, log(part1[index]) / (log(lengthLimit))) * part2[index];
			//double angle = ComplexFloat::Angle(value - prevValue, prevValue - prev2Value);
			//angle = angle / PI / 2.0 + 0.5;
			//angle = glm::fract(angle * 2); // for a better aspect ratio
			//UV = glm::vec2(angle, 1-ratio);
			return (double)i + ratio;
			//return (float)i;
		}
		ComplexFloat deviation = value - cycleValue;
		if (deviation.AbsoluteValueSqr() < minDeviationSqr)
		{
			return debugDeviations ? i : 0;
		}
		prev2Value = prevValue;
		prevValue = value;
		if (i % deviationCycle == 0) cycleValue = value;
	}
	return 0;
}