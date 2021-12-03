#pragma once
#include "FractalCommandListValidator.h"
#include <unordered_map>
#include <vector>

std::unordered_map<FractalCommandListValidator::Error, FractalCommandList> FractalCommandListFailureTests = {
	{
	FractalCommandListValidator::Error::NoError,
	FractalCommandList(0, 1, {(unsigned int)FractalCommand::multiply, 1, 1, 6, 1, 6,\
								(unsigned int)FractalCommand::add,      1, 1, 0, 3, 0 })
	},
}