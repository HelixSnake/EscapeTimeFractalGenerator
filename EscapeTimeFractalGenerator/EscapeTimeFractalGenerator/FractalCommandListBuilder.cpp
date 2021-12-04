#include "FractalCommandListBuilder.h"

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

FractalCommandList FractalCommandListBuilder::BuildCommandList() //Not implemented yet
{
	return FractalCommandList(constFloats.size(), constComplexFloats.size(), commands.size(), nullptr);
}