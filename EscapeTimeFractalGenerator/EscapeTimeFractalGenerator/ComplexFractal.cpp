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

double LogTower(double input, double power)
{
	return log(log(input)/log(power)) / log(power);
}

double ComplexFractal::CalculateEscapeTime(CF_Float x, CF_Float y, ComplexFloat* extraValues, CF_Float power)
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
			return (double)i + smoothValue;
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


double ComplexFractal::CalculateEscapeTime(FractalCommandListExecutor& startingFunction, FractalCommandListExecutor& recursiveFunction, CF_Float power)
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
	recursiveFunction.InitializeReturnValue(value);
	for (int i = 0; i < iterations; i++)
	{
		recursiveFunction.Execute();
		value = recursiveFunction.GetReturnValue();
		if (isnan(value.real) || isnan(value.imaginary)) return i; // if the values have gotten so extreme that a NAN showed up, just assume we diverged. Trust me, this is a good idea.
		double absValSqr = value.AbsoluteValueSqr();
		if (absValSqr > lengthLimitSqr)
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
			return (double)i + smoothValue;
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