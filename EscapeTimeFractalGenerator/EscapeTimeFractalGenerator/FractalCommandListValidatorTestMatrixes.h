#pragma once
#include "FractalCommandListValidator.h"
#include <unordered_map>
#include <vector>

std::vector<std::pair<FractalCommandListValidator::Error, FractalCommandList>> FractalCommandListFailureTests = {
	{
	FractalCommandListValidator::Error::NoError,
	FractalCommandList(0, 1, {(unsigned int)FractalCommand::move, 1, 3, 0, 3, 0})
	},
	{
	FractalCommandListValidator::Error::NoError,
	FractalCommandList(0, 1, {(unsigned int)FractalCommand::multiply, 1, 1, 6, 1, 6,\
								(unsigned int)FractalCommand::add,    1, 1, 0, 3, 0 })
	},
	{
	FractalCommandListValidator::Error::NoError,
	FractalCommandList(0, 1, {(unsigned int)FractalCommand::multiply, 1, 1, 12, 1, 12,\
								(unsigned int)FractalCommand::add,    1, 1, 0, 1, 12,\
								(unsigned int)FractalCommand::add,    1, 1, 6, 1, 12})
	},
	{
	FractalCommandListValidator::Error::CommandListLengthNotMultipleOf6,
	FractalCommandList(0, 1, {(unsigned int)FractalCommand::multiply, 1, 1, 6, 1, 6,\
								(unsigned int)FractalCommand::add,      1, 1, 0, 3, 0 , 3})
	},
	{
	FractalCommandListValidator::Error::FunctionOutOfBounds,
	FractalCommandList(0, 1, {(unsigned int)FractalCommand::multiply, 1, 1, 6, 1, 6,\
								(unsigned int)FractalCommand::NUM_ITEMS,      1, 1, 0, 3, 0})
	},
	{
	FractalCommandListValidator::Error::NoError,
	FractalCommandList(0, 1, {0, 1, 1, 6, 1, 6,\
								(unsigned int)FractalCommand::add,      1, 1, 0, 3, 0})
	},
	{
	FractalCommandListValidator::Error::ReturnTypeOutOfBounds,
	FractalCommandList(0, 1, {(unsigned int)FractalCommand::multiply, 2, 1, 6, 1, 6,\
								(unsigned int)FractalCommand::add,      1, 1, 0, 3, 0 })
	},
	{
	FractalCommandListValidator::Error::Arg1TypeOutOfBounds,
	FractalCommandList(0, 1, {(unsigned int)FractalCommand::multiply, 1, 1, 6, 1, 6,\
								(unsigned int)FractalCommand::add,      1, 4, 0, 3, 0 })
	},
	{
	FractalCommandListValidator::Error::Arg2TypeOutOfBounds,
	FractalCommandList(0, 1, {(unsigned int)FractalCommand::multiply, 1, 1, 6, 1, 6,\
								(unsigned int)FractalCommand::add,      1, 1, 0, 4, 0 })
	},
	{
	FractalCommandListValidator::Error::FunctionNotFound,
	FractalCommandList(0, 1, {(unsigned int)FractalCommand::ALL_NULL, 1, 1, 6, 1, 6,\
								(unsigned int)FractalCommand::add,      1, 1, 0, 4, 0 })
	},
	{
	FractalCommandListValidator::Error::Arg1IndexVarOutOfBounds,
	FractalCommandList(0, 1, {(unsigned int)FractalCommand::multiply, 1, 1, 12, 1, 6,\
								(unsigned int)FractalCommand::add,      1, 1, 0, 4, 0 })
	},
	{
	FractalCommandListValidator::Error::Arg2IndexVarOutOfBounds,
	FractalCommandList(0, 1, {(unsigned int)FractalCommand::multiply, 1, 1, 6, 1, 12,\
								(unsigned int)FractalCommand::add,      1, 1, 0, 4, 0 })
	},
	{
	FractalCommandListValidator::Error::Arg1IndexVarNotMultipleOf6,
	FractalCommandList(0, 1, {(unsigned int)FractalCommand::multiply, 1, 1, 7, 1, 6,\
								(unsigned int)FractalCommand::add,      1, 1, 0, 4, 0 })
	},
	{
	FractalCommandListValidator::Error::Arg2IndexVarNotMultipleOf6,
	FractalCommandList(0, 1, {(unsigned int)FractalCommand::multiply, 1, 1, 6, 1, 7,\
								(unsigned int)FractalCommand::add,      1, 1, 0, 4, 0 })
	},
	{
	FractalCommandListValidator::Error::Arg1IndexVarAheadButNotReturnValue,
	FractalCommandList(0, 1, {(unsigned int)FractalCommand::multiply, 1, 1, 0, 1, 6,\
								(unsigned int)FractalCommand::add,      1, 1, 0, 4, 0 })
	},
	{
	FractalCommandListValidator::Error::Arg1IndexVarAheadButNotReturnValue,
	FractalCommandList(0, 1, {(unsigned int)FractalCommand::multiply, 1, 1, 6, 1, 12,\
								(unsigned int)FractalCommand::add,    1, 1, 0, 4, 0,\
								(unsigned int)FractalCommand::add,    1, 1, 0, 4, 0 })
	},
	{
	FractalCommandListValidator::Error::Arg2IndexVarAheadButNotReturnValue,
	FractalCommandList(0, 1, {(unsigned int)FractalCommand::multiply, 1, 1, 6, 1, 0,\
								(unsigned int)FractalCommand::add,      1, 1, 0, 4, 0 })
	},
	{
	FractalCommandListValidator::Error::Arg2IndexVarAheadButNotReturnValue,
	FractalCommandList(0, 1, {(unsigned int)FractalCommand::multiply, 1, 1, 12, 1, 6,\
								(unsigned int)FractalCommand::add,    1, 1, 0, 4, 0,\
								(unsigned int)FractalCommand::add,    1, 1, 0, 4, 0 })
	},
	{
	FractalCommandListValidator::Error::Arg1IndexVarDoesntMatchType,
	FractalCommandList(0, 1, {(unsigned int)FractalCommand::multiply, 1, 1, 6, 1, 6,\
								(unsigned int)FractalCommand::add,      1, 0, 0, 1, 0 })
	},
	{
	FractalCommandListValidator::Error::Arg2IndexVarDoesntMatchType,
	FractalCommandList(0, 1, {(unsigned int)FractalCommand::multiply, 1, 1, 6, 0, 6,\
								(unsigned int)FractalCommand::add,      1, 1, 0, 1, 0 })
	},
	{
	FractalCommandListValidator::Error::Arg1IndexConstOutOfBounds,
	FractalCommandList(0, 6, {(unsigned int)FractalCommand::multiply, 1, 3, 6, 1, 6,\
								(unsigned int)FractalCommand::add,      1, 1, 0, 1, 0 })
	},
	{
	FractalCommandListValidator::Error::Arg2IndexConstOutOfBounds,
	FractalCommandList(0, 6, {(unsigned int)FractalCommand::multiply, 1, 1, 6, 3, 6,\
								(unsigned int)FractalCommand::add,      1, 1, 0, 1, 0 })
	},
	{
	FractalCommandListValidator::Error::Arg1IndexConstOutOfBounds,
	FractalCommandList(3, 6, {(unsigned int)FractalCommand::multiply, 1, 2, 4, 1, 6,\
								(unsigned int)FractalCommand::add,      1, 1, 0, 1, 0 })
	},
	{
	FractalCommandListValidator::Error::Arg2IndexConstOutOfBounds,
	FractalCommandList(3, 6, {(unsigned int)FractalCommand::multiply, 1, 1, 6, 2, 4,\
								(unsigned int)FractalCommand::add,      1, 1, 0, 1, 0 })
	},
};