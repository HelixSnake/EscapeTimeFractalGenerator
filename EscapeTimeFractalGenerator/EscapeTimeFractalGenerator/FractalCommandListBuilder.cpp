#include "FractalCommandListBuilder.h"
FractalCommandList FractalCommandListBuilder::BuildCommandList()
{
	return FractalCommandList(constFloats.size(), constComplexFloats.size(), commands.size(), nullptr);
}