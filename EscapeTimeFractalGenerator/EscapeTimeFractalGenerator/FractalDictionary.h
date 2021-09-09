#pragma once
#include "ComplexFractal.h"
#include <unordered_map>
#include <glm/common.hpp>

class FractalDictionary
{
public:
	static enum class FractalType
	{
		Mandelbrot,
		Julia,
		BurningShip,
		BurningJulia,
		NUM_ITEMS
	};
	const struct FractalTypeInfo
	{
		const char* name;
		const bool hasExtraValue;
		ComplexFractal::RecursiveFunction recursiveFunction;
		ComplexFractal::StartingValueFunction startingValueFunction;
	};

	static FractalTypeInfo GetInfo(FractalType type);

protected:

	static std::unordered_map<FractalType, FractalTypeInfo> dictionary;

	// Change these to modify the drawn fractal equations!
	static ComplexFloat FRACTAL_STARTING_FUNCTION_JULIA(ComplexFloat input, ComplexFloat extraValue)
	{
		return input;
	};
	static ComplexFloat FRACTAL_RECURSIVE_FUNCTION_JULIA(ComplexFloat input, ComplexFloat previousValue, ComplexFloat extraValue) {
		return previousValue * previousValue + extraValue;
	};
	static ComplexFloat FRACTAL_STARTING_FUNCTION_MANDEL(ComplexFloat input, ComplexFloat extraValue)
	{
		return ComplexFloat(0, 0);
	};
	static ComplexFloat FRACTAL_RECURSIVE_FUNCTION_MANDEL(ComplexFloat input, ComplexFloat previousValue, ComplexFloat extraValue) {
		return previousValue * previousValue + input;
	};
	static ComplexFloat FRACTAL_RECURSIVE_FUNCTION_BURNING_SHIP(ComplexFloat input, ComplexFloat previousValue, ComplexFloat extraValue) {
		return ComplexFloat::Abs(previousValue * previousValue + ComplexFloat(input.real, -input.imaginary));
	};
	static ComplexFloat FRACTAL_RECURSIVE_FUNCTION_BURNING_SHIP_JULIA(ComplexFloat input, ComplexFloat previousValue, ComplexFloat extraValue) {
		return ComplexFloat::Abs(previousValue * previousValue + ComplexFloat(extraValue.real, -extraValue.imaginary));
	};
};

