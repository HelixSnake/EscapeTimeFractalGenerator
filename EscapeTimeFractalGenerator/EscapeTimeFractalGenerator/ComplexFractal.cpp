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
	this->minDeviation = minDeviation;
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

inline double LogTower(double input, double power)
{
	return log(log(input)/log(power)) / log(power);
}

double inline CalculateEscapedValue(int iteration, double lengthLimit, ComplexFloat value, double power)
{
	double smoothValue = 0;
	if (power > 1.0)
	{
		double nextLengthLimit = pow(lengthLimit, power);
		double logNextLengthLimit = log(nextLengthLimit);

		smoothValue = (log(value.AbsoluteValue()) - logNextLengthLimit) / (log(lengthLimit) - logNextLengthLimit); // linear gradient between iterations
		double smoothPower = pow(power, 0.4); // magic number - moderately better results on high powers
		double logSmoothPower = log(smoothPower);
		double twoToPower = pow(2, smoothPower);
		double reverseLerp = smoothValue * (2 - twoToPower) + twoToPower;
		double dLogUpperLimit = log(log(twoToPower) / logSmoothPower) / logSmoothPower;
		smoothValue = dLogUpperLimit - log(log(reverseLerp) / logSmoothPower) / logSmoothPower;
	}
	else
	{
		smoothValue = log(lengthLimit) / log(value.AbsoluteValue()); // linear gradient between iterations
	}
	return (double)iteration + smoothValue;
}

double ComplexFractal::CalculateEscapeTime(CF_Float x, CF_Float y, ComplexFloat* extraValues, CF_Float power)
{
	//default algorithm for starting position
	ComplexFloat value = ComplexFloat(0, 0);
	if (startingValueFunction != nullptr)
	{ 
		value = startingValueFunction(ComplexFloat(x, y), extraValues, power);
	}
	// adding this constant to the value will insure that the fractal does not escape the first iteration due to the minimum deviation
	ComplexFloat cycleValue = value + ComplexFloat(minDeviation, minDeviation);
	CF_Float lengthLimitSqr = lengthLimit * lengthLimit;
	int setCycleValueNext = 0;
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
		if (value.AbsoluteValueSqr() > lengthLimitSqr)
		{ 
			return CalculateEscapedValue(i, lengthLimit, value, power);
		}
		ComplexFloat deviation = value - cycleValue;
		if (abs(deviation.real) < minDeviation && abs(deviation.imaginary) < minDeviation)
		{
			return debugDeviations? i : 0;
		}
		if (i == setCycleValueNext)
		{
			cycleValue = value;
			setCycleValueNext += deviationCycle;
		}
	}
	return 0;
}


double ComplexFractal::CalculateEscapeTime(FractalCommandListExecutor& startingFunction, FractalCommandListExecutor& recursiveFunction, CF_Float power)
{//default algorithm for starting position
	ComplexFloat value = ComplexFloat(0, 0);
	startingFunction.Execute();
	value = startingFunction.GetReturnValue();
	// adding this constant to the value will insure that the fractal does not escape the first iteration due to the minimum deviation
	ComplexFloat cycleValue = value + ComplexFloat(minDeviation, minDeviation);
	recursiveFunction.InitializeReturnValue(value);
	CF_Float lengthLimitSqr = lengthLimit * lengthLimit;
	int setCycleValueNext = 0;
	for (int i = 0; i < iterations; i++)
	{
		recursiveFunction.Execute();
		value = recursiveFunction.GetReturnValue();
		if (isnan(value.real) || isnan(value.imaginary)) return i; // if the values have gotten so extreme that a NAN showed up, just assume we diverged. Trust me, this is a good idea.
		if (value.AbsoluteValueSqr() > lengthLimitSqr)
		{
			return CalculateEscapedValue(i, lengthLimit, value, power);
		}
		ComplexFloat deviation = value - cycleValue;
		if (abs(deviation.real) < minDeviation && abs(deviation.imaginary) < minDeviation)
		{
			return debugDeviations ? i : 0;
		}
		if (i == setCycleValueNext)
		{
			cycleValue = value;
			setCycleValueNext += deviationCycle;
		}
	}
	return 0;
}	