#pragma once
#include "ComplexFloat.h"
#include "FractalCommandList.h"

enum struct FractalCommand
{
	move,
	add, 
	subtract,
	multiply,
	divide,
	power,
	ln,
	sin,
	cos,
	tan,
	sinh,
	cosh,
	tanh,
	//asin,
	magnitude,
	magnitudesqr,
	normalize,
	getx,
	gety,
	floatstocomplex,
	toimaginary,
	dot,
	arg,
	step,
	ALL_NULL, //used for testing; no delegates will exist for this command
	NUM_ITEMS
};
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
	//For functions with a single argument, both argument types will be the same.
	
	static const unsigned long long ARRAY_LENGTH = (unsigned long long)FractalCommand::NUM_ITEMS;

	FloatFloatFloat FFF_Functions[ARRAY_LENGTH];
	FloatFloatCF FFC_Functions[ARRAY_LENGTH];
	FloatCFFloat FCF_Functions[ARRAY_LENGTH];
	FloatCFCF FCC_Functions[ARRAY_LENGTH];
	CFFloatFloat CFF_Functions[ARRAY_LENGTH];
	CFFloatCF CFC_Functions[ARRAY_LENGTH];
	CFCFFloat CCF_Functions[ARRAY_LENGTH];
	CFCFCF CCC_Functions[ARRAY_LENGTH];

	const char* commandNames[ARRAY_LENGTH];
	int commandInputs[ARRAY_LENGTH];
	int resultTypes[ARRAY_LENGTH][2][2];

	bool IsDelegatePointerNull(int index, int returnType, int arg1type, int arg2type);
private:
	void GenerateKnownCommandDelegates();
	void GenerateKnownCommandNames();
	void GenerateKnownCommandInputs();
	void GenerateKnownCommandReturnTypes();
};

