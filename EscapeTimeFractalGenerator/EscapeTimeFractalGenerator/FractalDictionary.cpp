#include "FractalDictionary.h"

std::unordered_map<FractalDictionary::FractalType, FractalDictionary::FractalTypeInfo> FractalDictionary::dictionary = {
	{
		FractalDictionary::FractalType::Mandelbrot, 
		{"Mandelbrot", false, &FractalDictionary::FRACTAL_RECURSIVE_FUNCTION_MANDEL, &FractalDictionary::FRACTAL_STARTING_FUNCTION_MANDEL}
	},
	{
		FractalDictionary::FractalType::Julia,
		{"Julia", true, &FractalDictionary::FRACTAL_RECURSIVE_FUNCTION_JULIA, &FractalDictionary::FRACTAL_STARTING_FUNCTION_JULIA}
	},
	{
		FractalDictionary::FractalType::BurningShip,
		{"Burning Ship", false, &FractalDictionary::FRACTAL_RECURSIVE_FUNCTION_BURNING_SHIP, &FractalDictionary::FRACTAL_STARTING_FUNCTION_MANDEL}
	},
	{
		FractalDictionary::FractalType::BurningJulia,
		{"Burning Ship Julia", true, &FractalDictionary::FRACTAL_RECURSIVE_FUNCTION_BURNING_SHIP_JULIA, &FractalDictionary::FRACTAL_STARTING_FUNCTION_JULIA}
	}
};

FractalDictionary::FractalTypeInfo FractalDictionary::GetInfo(FractalDictionary::FractalType type)
{
	auto itr = dictionary.find(type);
	if (itr != dictionary.end())
	{
		return itr->second;
	}
	else
	{
		return { "", false, nullptr, nullptr};
	}
}