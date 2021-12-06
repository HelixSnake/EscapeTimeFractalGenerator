#pragma once
#include "ComplexFractal.h"
#include <unordered_map>
#include <glm/common.hpp>
#include <cstdarg>

class FractalDictionary
{
public:
	enum class FractalType
	{
		Mandelbrot,
		Julia,
		BurningShip,
		BurningJulia,
		ReflectedMandelbrot,
		ReflectedJulia,
		SpunMandelbrot,
		SpunJulia,
		Peacock,
		PeacockJulia,
		CosineMandelbrot,
		CosineJulia,
		ReciprocalMandelbrot,
		ReciprocalJulia,
		SuperMandelbrot,
		SuperJulia,
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
	static ComplexFloat FRACTAL_STARTING_FUNCTION_JULIA(ComplexFloat input, ComplexFloat* extraValues, int power)
	{
		return input;
	};
	static ComplexFloat FRACTAL_RECURSIVE_FUNCTION_JULIA(ComplexFloat input, ComplexFloat previousValue, ComplexFloat* extraValues, int power) {
		
		return previousValue.Power(power) + extraValues[0];
	};
	static ComplexFloat FRACTAL_STARTING_FUNCTION_MANDEL(ComplexFloat input, ComplexFloat* extraValues, int power)
	{
		return ComplexFloat(0, 0);
	};
	static ComplexFloat FRACTAL_RECURSIVE_FUNCTION_MANDEL(ComplexFloat input, ComplexFloat previousValue, ComplexFloat* extraValues, int power) {
		return previousValue.Power(power) + input;
	};
	static ComplexFloat FRACTAL_RECURSIVE_FUNCTION_BURNING_SHIP(ComplexFloat input, ComplexFloat previousValue, ComplexFloat* extraValues, int power) {
		return (previousValue.Power(power) + ComplexFloat(input.real, -input.imaginary)).Abs();
	};
	static ComplexFloat FRACTAL_RECURSIVE_FUNCTION_BURNING_SHIP_JULIA(ComplexFloat input, ComplexFloat previousValue, ComplexFloat* extraValues, int power) {
		return (previousValue.Power(power) + ComplexFloat(extraValues[0].real, -extraValues[0].imaginary)).Abs();
	};
	static ComplexFloat FRACTAL_RECURSIVE_FUNCTION_REFLECTED_MANDEL(ComplexFloat input, ComplexFloat previousValue, ComplexFloat* extraValues, int power) {
		ComplexFloat newInput = previousValue.Power(power);
		CF_Float dot = ComplexFloat::Dot(newInput, extraValues[1]);
		newInput = newInput + input;
		if (dot < 0)
		{
			newInput = newInput - extraValues[1] * dot * 2;
		}
		return newInput;
	};
	static ComplexFloat FRACTAL_RECURSIVE_FUNCTION_REFLECTED_JULIA(ComplexFloat input, ComplexFloat previousValue, ComplexFloat* extraValues, int power) {
		ComplexFloat newInput = previousValue.Power(power);
		CF_Float dot = ComplexFloat::Dot(newInput, extraValues[1]);
		newInput = newInput + extraValues[0];
		if (dot < 0)
		{
			newInput = newInput - extraValues[1] * dot * 2;
		}
		return newInput;
	};
	static ComplexFloat FRACTAL_RECURSIVE_FUNCTION_SPUN_MANDEL(ComplexFloat input, ComplexFloat previousValue, ComplexFloat* extraValues, int power) {
		ComplexFloat newInput = previousValue.Power(power) + input;
		newInput = newInput + newInput.Normalized() * extraValues[1];
		return newInput;
	};
	static ComplexFloat FRACTAL_RECURSIVE_FUNCTION_SPUN_JULIA(ComplexFloat input, ComplexFloat previousValue, ComplexFloat* extraValues, int power) {
		ComplexFloat newInput = previousValue.Power(power) + extraValues[0];
		newInput = newInput + newInput.Normalized() * extraValues[1];
		return newInput;
	};
	static ComplexFloat FRACTAL_RECURSIVE_FUNCTION_BACKWARDS_MANDEL(ComplexFloat input, ComplexFloat previousValue, ComplexFloat* extraValues, int power) {
		ComplexFloat newInput = ComplexFloat::Power(extraValues[1].real, previousValue).Power(power);
		newInput = newInput + input;
		return newInput;
	};
	static ComplexFloat FRACTAL_RECURSIVE_FUNCTION_BACKWARDS_JULIA(ComplexFloat input, ComplexFloat previousValue, ComplexFloat* extraValues, int power) {
		ComplexFloat newInput = ComplexFloat::Power(extraValues[1].real, previousValue).Power(power);
		newInput = newInput + extraValues[0];
		return newInput;
	};
	static ComplexFloat FRACTAL_RECURSIVE_FUNCTION_COSINE_MANDEL(ComplexFloat input, ComplexFloat previousValue, ComplexFloat* extraValues, int power) {
		ComplexFloat newInput = ComplexFloat(cosl(previousValue.real) * coshl(previousValue.imaginary), -sinl(previousValue.real) * sinhl(previousValue.imaginary));
		newInput = newInput.Power(power);
		newInput = newInput + input;
		return newInput;
	};
	static ComplexFloat FRACTAL_RECURSIVE_FUNCTION_COSINE_JULIA(ComplexFloat input, ComplexFloat previousValue, ComplexFloat* extraValues, int power) {
		ComplexFloat newInput = ComplexFloat(cosl(previousValue.real) * coshl(previousValue.imaginary), -sinl(previousValue.real) * sinhl(previousValue.imaginary));
		newInput = newInput.Power(power);
		newInput = newInput + extraValues[0];
		return newInput;
	};
	static ComplexFloat FRACTAL_RECURSIVE_FUNCTION_RECIPROCAL_MANDEL(ComplexFloat input, ComplexFloat previousValue, ComplexFloat* extraValues, int power) {
		ComplexFloat newInput = previousValue.Power(power).Reciprocal();
		newInput = newInput + input;
		return newInput;
	};
	static ComplexFloat FRACTAL_RECURSIVE_FUNCTION_RECIPROCAL_JULIA(ComplexFloat input, ComplexFloat previousValue, ComplexFloat* extraValues, int power) {
		ComplexFloat newInput = previousValue.Power(power).Reciprocal();
		newInput = newInput + extraValues[0];
		return newInput;
	};
	static ComplexFloat FRACTAL_RECURSIVE_FUNCTION_SUPER_MANDEL(ComplexFloat input, ComplexFloat previousValue, ComplexFloat* extraValues, int power) {
		ComplexFloat newInput = ComplexFloat::Power(previousValue, extraValues[1]);
		newInput = newInput + input;
		return newInput;
	};
	static ComplexFloat FRACTAL_RECURSIVE_FUNCTION_SUPER_JULIA(ComplexFloat input, ComplexFloat previousValue, ComplexFloat* extraValues, int power) {
		ComplexFloat newInput = ComplexFloat::Power(previousValue, extraValues[1]);
		newInput = newInput + extraValues[0];
		return newInput;
	};
};

