#pragma once
#include "ComplexFractal.h"
#include <unordered_map>
#include <glm/common.hpp>
#include <cstdarg>

class FractalDictionary
{
public:
	static enum class FractalType
	{
		Mandelbrot,
		Julia,
		BurningShip,
		BurningJulia,
		ReflectedMandelbrot,
		ReflectedJulia,
		NUM_ITEMS
	};
	const struct FractalTypeInfo
	{
		const char* name;
		const int extraValues;
		ComplexFractal::RecursiveFunction recursiveFunction;
		ComplexFractal::StartingValueFunction startingValueFunction;
	};

	static FractalTypeInfo GetInfo(FractalType type);
	static int GetMaxValues();

protected:

	static std::unordered_map<FractalType, FractalTypeInfo> dictionary;

	// Change these to modify the drawn fractal equations!
	// NOTE: BOUNDS CHECKING IS NOT PERFORMED IN ORDER TO SAVE COMPUTATION TIME. IF YOU CRASH HERE IT IS BECAUSE THE PROGRAM DID NOT PROPERLY GENERATE THE EXTRA VALUES ARRAY.
	// MAKE SURE THAT THE NUMBER OF VALUES IN THE FRACTAL DICTIONARY IN FRACTALDICTIONARY.CPP IS CORRECT FOR EACH FUNCTION USED AND THAT THIS PROPEGATES TO
	// A PROPERLY SIZED ARRAY IN THE FRACTALDRAWER DRAW() CALL.
	static ComplexFloat FRACTAL_STARTING_FUNCTION_JULIA(ComplexFloat input, ComplexFloat* extraValues)
	{
		return input;
	};
	static ComplexFloat FRACTAL_RECURSIVE_FUNCTION_JULIA(ComplexFloat input, ComplexFloat previousValue, ComplexFloat* extraValues) {
		return previousValue * previousValue + extraValues[0];
	};
	static ComplexFloat FRACTAL_STARTING_FUNCTION_MANDEL(ComplexFloat input, ComplexFloat* extraValues)
	{
		return ComplexFloat(0, 0);
	};
	static ComplexFloat FRACTAL_RECURSIVE_FUNCTION_MANDEL(ComplexFloat input, ComplexFloat previousValue, ComplexFloat* extraValues) {
		return previousValue * previousValue + input;
	};
	static ComplexFloat FRACTAL_RECURSIVE_FUNCTION_BURNING_SHIP(ComplexFloat input, ComplexFloat previousValue, ComplexFloat* extraValues) {
		return ComplexFloat::Abs(previousValue * previousValue + ComplexFloat(input.real, -input.imaginary));
	};
	static ComplexFloat FRACTAL_RECURSIVE_FUNCTION_BURNING_SHIP_JULIA(ComplexFloat input, ComplexFloat previousValue, ComplexFloat* extraValues) {
		return ComplexFloat::Abs(previousValue * previousValue + ComplexFloat(extraValues[0].real, -extraValues[0].imaginary));
	};
	static ComplexFloat FRACTAL_RECURSIVE_FUNCTION_REFLECTED_MANDEL(ComplexFloat input, ComplexFloat previousValue, ComplexFloat* extraValues) {
		ComplexFloat newInput = previousValue * previousValue + input;
		CF_Float dot = ComplexFloat::Dot(newInput, extraValues[1]);
		if (dot < 0)
		{
			newInput = newInput - extraValues[1] * dot * 2;
		}
		return newInput;
	};
	static ComplexFloat FRACTAL_RECURSIVE_FUNCTION_REFLECTED_JULIA(ComplexFloat input, ComplexFloat previousValue, ComplexFloat* extraValues) {
		ComplexFloat newInput = previousValue * previousValue + extraValues[0];
		CF_Float dot = ComplexFloat::Dot(newInput, extraValues[1]);
		if (dot < 0)
		{
			newInput = newInput - extraValues[1] * dot * 2;
		}
		return newInput;
	};
};

