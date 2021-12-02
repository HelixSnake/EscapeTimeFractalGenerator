#include "FractalCommandDelegates.h"
#include <string>



FractalCommandDelegates::FractalCommandDelegates()
{
	memset(FFF_Functions, 0, ARRAY_LENGTH * sizeof(FloatFloatFloat));
	memset(FFC_Functions, 0, ARRAY_LENGTH * sizeof(FloatFloatCF));
	memset(FCF_Functions, 0, ARRAY_LENGTH * sizeof(FloatCFFloat));
	memset(FCC_Functions, 0, ARRAY_LENGTH * sizeof(FloatCFCF));
	memset(CFF_Functions, 0, ARRAY_LENGTH * sizeof(CFFloatFloat));
	memset(CFC_Functions, 0, ARRAY_LENGTH * sizeof(CFFloatCF));
	memset(CCF_Functions, 0, ARRAY_LENGTH * sizeof(CFCFFloat));
	memset(CCC_Functions, 0, ARRAY_LENGTH * sizeof(CFCFCF));
}