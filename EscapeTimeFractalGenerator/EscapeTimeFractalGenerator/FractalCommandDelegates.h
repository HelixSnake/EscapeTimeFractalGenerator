#pragma once
#include "ComplexFloat.h"
#include "FractalCommandList.h"
class FractalCommandDelegates //A generated set of function pointer arrays. Used for calling the functions indicated by the commands in FractalCommandList. You only need one instance of this class for the lifetime of the program.
//Once you have created an instance of this class, do not delete it!
{
public:

	FractalCommandDelegates();

	typedef CF_Float (*FloatFloatFloat) (CF_Float first, CF_Float second);
	typedef CF_Float(*FloatFloatCF) (CF_Float first, ComplexFloat second);
	typedef CF_Float(*FloatCFFloat) (ComplexFloat first, CF_Float second);
	typedef CF_Float(*FloatCFCF) (ComplexFloat first, ComplexFloat second);
	typedef ComplexFloat(*CFFloatFloat) (CF_Float first, CF_Float second);
	typedef ComplexFloat(*CFFloatCF) (CF_Float first, ComplexFloat second);
	typedef ComplexFloat(*CFCFFloat) (ComplexFloat first, CF_Float second);
	typedef ComplexFloat(*CFCFCF) (ComplexFloat first, ComplexFloat second);
	
	static const unsigned long long ARRAY_LENGTH = (unsigned long long)FractalCommandList::Command::NUM_ITEMS;

	FloatFloatFloat FFF_Functions[ARRAY_LENGTH];
	FloatFloatCF FFC_Functions[ARRAY_LENGTH];
	FloatCFFloat FCF_Functions[ARRAY_LENGTH];
	FloatCFCF FCC_Functions[ARRAY_LENGTH];
	CFFloatFloat CFF_Functions[ARRAY_LENGTH];
	CFFloatCF CFC_Functions[ARRAY_LENGTH];
	CFCFFloat CCF_Functions[ARRAY_LENGTH];
	CFCFCF CCC_Functions[ARRAY_LENGTH];
private:
	void GenerateKnownCommandDelegates();
};

