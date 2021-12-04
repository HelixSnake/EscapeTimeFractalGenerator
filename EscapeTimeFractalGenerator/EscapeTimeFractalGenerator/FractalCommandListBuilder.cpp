#include "FractalCommandListBuilder.h"

const char* const FractalCommandListBuilder::DataTypeStrings[2] =
{
	"Float",
	"ComplexFloat"
};
const char* const FractalCommandListBuilder::SourceStrings[4] =
{
	"Variables",
	"Constants",
	"Input",
	"PreviousValue"
};
bool FractalCommandListBuilder::AddCommand(unsigned int index, Command command)
{
	if (index > commands.size()) return false;
	commands.insert(commands.begin() + index, command);
	return true;
}
bool FractalCommandListBuilder::AddConstFloat(unsigned int index, CF_Float value)
{
	if (index > constFloats.size()) return false;
	constFloats.insert(constFloats.begin() + index, value);
	return true;
}
bool FractalCommandListBuilder::AddConstComplexFloat(unsigned int index, ComplexFloat value)
{
	if (index > constComplexFloats.size()) return false;
	constComplexFloats.insert(constComplexFloats.begin() + index, value);
	return true;
}
bool FractalCommandListBuilder::DeleteCommand(unsigned int index)
{
	if (index >= commands.size()) return false;
	commands.erase(commands.begin() + index);
	return true;
}
bool FractalCommandListBuilder::DeleteConstFloat(unsigned int index)
{
	if (index >= constFloats.size()) return false;
	constFloats.erase(constFloats.begin() + index);
	return true;
}
bool FractalCommandListBuilder::DeleteConstComplexFloat(unsigned int index)
{
	if (index >= constComplexFloats.size()) return false;
	constComplexFloats.erase(constComplexFloats.begin() + index);
	return true;
}
bool FractalCommandListBuilder::SetCommand(unsigned int index, Command command)
{
	if (index >= commands.size()) return false;
	commands[index] = (command);
	return true;
}
bool FractalCommandListBuilder::SetConstFloat(unsigned int index, CF_Float value)
{
	if (index >= constFloats.size()) return false;
	constFloats[index] = (value);
	return true;
}
bool FractalCommandListBuilder::SetConstComplexFloat(unsigned int index, ComplexFloat value)
{
	if (index >= constComplexFloats.size()) return false;
	constComplexFloats[index] = (value);
	return true;
}
const std::vector<FractalCommandListBuilder::Command> FractalCommandListBuilder::GetCommands()
{
	return commands;
}
const std::vector<CF_Float>  FractalCommandListBuilder::GetConstFloats()
{
	return constFloats;
}
const std::vector<ComplexFloat>  FractalCommandListBuilder::GetConstComplexFloats()
{
	return constComplexFloats;
}

FractalCommandList FractalCommandListBuilder::BuildCommandList()
{
	unsigned int* ints = new unsigned int[commands.size() * 6];
	for (int i = 0; i < commands.size(); i++)
	{
		int currentIndex = i * 6;
		ints[currentIndex] = (unsigned int)commands[i].function;
		ints[currentIndex + 1] = (unsigned int)commands[i].outputDatatype;
		switch (commands[i].firstArgSource)
		{
		case Source::Variables:
			ints[currentIndex + 2] = commands[i].firstArgDatatype == Datatype::Float ? 0 : 1;
			ints[currentIndex + 3] = commands[i].firstArgindex * 6;
			break;
		case Source::Constants:
			ints[currentIndex + 2] = commands[i].firstArgDatatype == Datatype::Float ? 2 : 3;
			ints[currentIndex + 3] = commands[i].firstArgindex + (commands[i].firstArgDatatype == Datatype::ComplexFloat ? 1 : 0);
			break;
		case Source::Input:
			ints[currentIndex + 2] = 3; 
			ints[currentIndex + 3] = 0;
			break;
		case Source::PreviousValue:
			ints[currentIndex + 2] = 1;
			ints[currentIndex + 3] = commands.size() * 6 - 6;
			break;
		}
		switch (commands[i].secondArgSource)
		{
		case Source::Variables:
			ints[currentIndex + 4] = commands[i].secondArgDatatype == Datatype::Float ? 0 : 1;
			ints[currentIndex + 5] = commands[i].secondArgindex * 6;
			break;
		case Source::Constants:
			ints[currentIndex + 4] = commands[i].secondArgDatatype == Datatype::Float ? 2 : 3;
			ints[currentIndex + 5] = commands[i].secondArgindex + (commands[i].secondArgDatatype == Datatype::ComplexFloat ? 1 : 0);
			break;
		case Source::Input:
			ints[currentIndex + 4] = 3;
			ints[currentIndex + 5] = 0;
			break;
		case Source::PreviousValue:
			ints[currentIndex + 4] = 1;
			ints[currentIndex + 5] = commands.size() * 6 - 6;
			break;
		}
	}
	return FractalCommandList(constFloats.size(), constComplexFloats.size() + 1, commands.size() * 6, ints);
}