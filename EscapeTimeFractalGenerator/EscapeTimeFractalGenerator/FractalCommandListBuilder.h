#pragma once
#include <vector>
#include "FractalCommandList.h"
#include "FractalCommandDelegates.h"
class FractalCommandListBuilder
{
public:
	enum class Datatype
	{
		Float,
		ComplexFloat,
		NUM_ITEMS
	};
	static const char* const DataTypeStrings[2];
	enum class Source
	{
		Variables,
		Constants,
		Input,
		PreviousValue,
		NUM_ITEMS
	};
	static const char* const SourceStrings[4];
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
	bool AddCommand(unsigned int index, Command command);
	bool AddCommand(unsigned int index);
	bool AddConstFloat(unsigned int index, CF_Float value);
	bool AddConstComplexFloat(unsigned int index, ComplexFloat value);
	bool DeleteCommand(unsigned int index);
	bool DeleteConstFloat(unsigned int index);
	bool DeleteConstComplexFloat(unsigned int index);
	bool SetCommand(unsigned int index, Command command);
	bool SetConstFloat(unsigned int index, CF_Float value);
	bool SetConstComplexFloat(unsigned int index, ComplexFloat value);
	const std::vector<Command> GetCommands();
	const std::vector<CF_Float>  GetConstFloats();
	const std::vector<ComplexFloat>  GetConstComplexFloats();


	FractalCommandList BuildCommandList();

	private:
	std::vector<CF_Float> constFloats;
	std::vector<ComplexFloat> constComplexFloats;
	std::vector<Command> commands;
};

