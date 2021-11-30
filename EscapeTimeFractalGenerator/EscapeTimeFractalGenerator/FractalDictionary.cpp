#include "FractalDictionary.h"

std::unordered_map<FractalDictionary::FractalType, FractalDictionary::FractalTypeInfo> FractalDictionary::dictionary = {
	{
		FractalDictionary::FractalType::Mandelbrot, 
		{"Mandelbrot", 0, &FractalDictionary::FRACTAL_RECURSIVE_FUNCTION_MANDEL, &FractalDictionary::FRACTAL_STARTING_FUNCTION_MANDEL}
	},
	{
		FractalDictionary::FractalType::Julia,
		{"Julia", 1, &FractalDictionary::FRACTAL_RECURSIVE_FUNCTION_JULIA, &FractalDictionary::FRACTAL_STARTING_FUNCTION_JULIA}
	},
	{
		FractalDictionary::FractalType::BurningShip,
		{"Burning Ship", 0, &FractalDictionary::FRACTAL_RECURSIVE_FUNCTION_BURNING_SHIP, &FractalDictionary::FRACTAL_STARTING_FUNCTION_MANDEL}
	},
	{
		FractalDictionary::FractalType::BurningJulia,
		{"Burning Ship Julia", 1, &FractalDictionary::FRACTAL_RECURSIVE_FUNCTION_BURNING_SHIP_JULIA, &FractalDictionary::FRACTAL_STARTING_FUNCTION_JULIA}
	},
	{
		FractalDictionary::FractalType::ReflectedMandelbrot,
		{"Distorted Mandelbrot", 2, &FractalDictionary::FRACTAL_RECURSIVE_FUNCTION_REFLECTED_MANDEL, &FractalDictionary::FRACTAL_STARTING_FUNCTION_MANDEL}
	},
	{
		FractalDictionary::FractalType::ReflectedJulia,
		{"Distorted Julia", 2, &FractalDictionary::FRACTAL_RECURSIVE_FUNCTION_REFLECTED_JULIA, &FractalDictionary::FRACTAL_STARTING_FUNCTION_JULIA}
	},
	{
		FractalDictionary::FractalType::SpunMandelbrot,
		{"Spun Mandelbrot", 2, &FractalDictionary::FRACTAL_RECURSIVE_FUNCTION_SPUN_MANDEL, &FractalDictionary::FRACTAL_STARTING_FUNCTION_MANDEL}
	},
	{
		FractalDictionary::FractalType::SpunJulia,
		{"Spun Julia", 2, &FractalDictionary::FRACTAL_RECURSIVE_FUNCTION_SPUN_JULIA, &FractalDictionary::FRACTAL_STARTING_FUNCTION_JULIA}
	},
	{
		FractalDictionary::FractalType::Peacock,
		{"Peacock Fractal", 2, &FractalDictionary::FRACTAL_RECURSIVE_FUNCTION_BACKWARDS_MANDEL, &FractalDictionary::FRACTAL_STARTING_FUNCTION_MANDEL}
	},
	{
		FractalDictionary::FractalType::PeacockJulia,
		{"Peacock Julia", 2, &FractalDictionary::FRACTAL_RECURSIVE_FUNCTION_BACKWARDS_JULIA, &FractalDictionary::FRACTAL_STARTING_FUNCTION_JULIA}
	},
	{
		FractalDictionary::FractalType::CosineMandelbrot,
		{"Cosine Fractal Mandelbrot", 1, &FractalDictionary::FRACTAL_RECURSIVE_FUNCTION_COSINE_MANDEL, &FractalDictionary::FRACTAL_STARTING_FUNCTION_MANDEL}
	},
	{
		FractalDictionary::FractalType::CosineJulia,
		{"Cosine Fractal Julia", 1, &FractalDictionary::FRACTAL_RECURSIVE_FUNCTION_COSINE_JULIA, &FractalDictionary::FRACTAL_STARTING_FUNCTION_JULIA}
	},
	{
		FractalDictionary::FractalType::ReciprocalMandelbrot,
		{"Reciprocal Mandelbrot", 1, &FractalDictionary::FRACTAL_RECURSIVE_FUNCTION_RECIPROCAL_MANDEL, &FractalDictionary::FRACTAL_STARTING_FUNCTION_MANDEL}
	},
	{
		FractalDictionary::FractalType::ReciprocalJulia,
		{"Reciprocal Julia", 1, &FractalDictionary::FRACTAL_RECURSIVE_FUNCTION_RECIPROCAL_JULIA, &FractalDictionary::FRACTAL_STARTING_FUNCTION_JULIA}
	},
	{
		FractalDictionary::FractalType::SuperMandelbrot,
		{"Super Mandelbrot", 2, &FractalDictionary::FRACTAL_RECURSIVE_FUNCTION_SUPER_MANDEL, &FractalDictionary::FRACTAL_STARTING_FUNCTION_MANDEL}
	},
	{
		FractalDictionary::FractalType::SuperJulia,
		{"Super Julia", 2, &FractalDictionary::FRACTAL_RECURSIVE_FUNCTION_SUPER_JULIA, &FractalDictionary::FRACTAL_STARTING_FUNCTION_JULIA}
	},
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

int FractalDictionary::GetMaxValues()
{
	int mostExtraValues = 0;
	for (auto itr = dictionary.begin(); itr != dictionary.end(); itr++)
	{
		mostExtraValues = std::max(mostExtraValues, itr->second.extraValues);
	}
	return mostExtraValues;
}