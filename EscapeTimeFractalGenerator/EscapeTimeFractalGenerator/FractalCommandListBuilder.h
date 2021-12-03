#pragma once
#include <vector>
#include "FractalCommandList.h"
class FractalCommandListBuilder
{
	enum class Datatype
	{
		Float,
		ComplexFloat
	};
	enum class Source
	{
		Variables,
		Constants,
		Input,
		PreviousValue
	};
	struct Command
	{
		FractalCommand function;
		Datatype outputDatatype;
		Datatype firstArgDatatype;
		Datatype secondArgDatatype;
		Source firstArgSource;
		Source secondArgSource;
		int firstArgindex;
		int secondArgindex;
	};
	FractalCommandList BuildCommandList();
	private:
	std::vector<CF_Float> constFloats;
	std::vector<ComplexFloat> constComplexFloats;
	std::vector<Command> commands;
};

