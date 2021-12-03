#include "test.h"
#include "FractalCommandList.h"
#include "FractalCommandListExecutor.h"

// Function to add two integers 
int addIntegers(const int& a, const int& b)
{
	return a + b;
}

// Test case to check commutative property of addIntegers 
BEGIN_TEST(AddProps, Commutative)
{
	EXPECT_EQ(addIntegers(3, 5), addIntegers(5, 3))
}END_TEST

//Fractal Command List Tests -------------------------------------------------------------------------------------------------------------------------
BEGIN_TEST(FractalCommandList, ArrayConstruction)
	{
		unsigned int ints[5] = { 2, 4, 5, 7, 8 };
		FractalCommandList fcl(0, 0, 5, ints);
		for (int i = 0; i < 5; i++)
		{
			EXPECT_EQ(ints[i], fcl.GetCommandList()[i])
		}
	}END_TEST

//Fractal Command Delegate Tests -------------------------------------------------------------------------------------------------------------------------
BEGIN_TEST(FractalCommandDelegate, RunCFCFCFMove)
	{
		ComplexFloat first = ComplexFloat(1, 2);
		ComplexFloat expectedResult = ComplexFloat(1, 2);
		FractalCommandDelegates delegates;
		ComplexFloat result = delegates.CCC_Functions[(int)FractalCommand::move](first, ComplexFloat(0,0));
		EXPECT_EQ(expectedResult, result)
	}END_TEST

BEGIN_TEST(FractalCommandDelegate, RunCFCFCFAdd)
	{
		ComplexFloat first = ComplexFloat(1, 2);
		ComplexFloat second = ComplexFloat(3, 4);
		ComplexFloat expectedResult = ComplexFloat(4, 6);
		FractalCommandDelegates delegates;
		ComplexFloat result = delegates.CCC_Functions[(int)FractalCommand::add](first, second);
		EXPECT_EQ(expectedResult, result)
	}END_TEST

BEGIN_TEST(FractalCommandDelegate, RunCFCFCFMult)
		{
			ComplexFloat first = ComplexFloat(1, 2);
			ComplexFloat second = ComplexFloat(3, 4);
			ComplexFloat expectedResult = ComplexFloat(-5, 10);
			FractalCommandDelegates delegates;
			ComplexFloat result = delegates.CCC_Functions[(int)FractalCommand::multiply](first, second);
			EXPECT_EQ(expectedResult, result)
		}END_TEST


//Fractal Command List Executor Tests -------------------------------------------------------------------------------------------------------------------------

BEGIN_TEST(FractalCommandExecutor, TestNullDelegates)
{
	FractalCommandDelegates delegates;
	for (int i = 0; i <= 1; i++)
		for (int j = 0; j <= 3; j++)
			for (int k = 0; k <= 3; k++)
			{
				unsigned int ints[6] = { (int)FractalCommand::ALL_NULL, i, j, 0, k, 0 };
				FractalCommandList commandList = FractalCommandList(1, 1, 1, ints);
				FractalCommandListExecutor executor = FractalCommandListExecutor(commandList, &delegates);
				EXPECT_EQ(executor.Execute(), 1);
			}
}END_TEST

BEGIN_TEST(FractalCommandExecutor, ExecuteMandelbrotFunction)
{
	// construct command list array for mandelbrot function, n = (n - 1) * (n - 1) + c
	unsigned int ints[12] = { (int)FractalCommand::multiply, 1, 1, 6, 1, 6,\
								(int)FractalCommand::add,      1, 1, 0, 3, 0 };
	FractalCommandList fcl(0, 1, 12, ints);
	FractalCommandDelegates delegates;
	FractalCommandListExecutor executor = FractalCommandListExecutor(fcl, &delegates);
	executor.SetConstantComplexFloat(0, ComplexFloat(5, 7));
	executor.InitializeReturnValue(ComplexFloat(4, 3));
	executor.Execute();
	EXPECT_EQ(executor.GetReturnValue(), ComplexFloat(12, 31))
}END_TEST

//Run all -------------------------------------------------------------------------------------------------------------------------
int RunAllUnitTests()
	{
		RUN_TEST(AddProps, Commutative)
			RUN_TEST(FractalCommandList, ArrayConstruction)
			RUN_TEST(FractalCommandExecutor, TestNullDelegates)
			RUN_TEST(FractalCommandExecutor, ExecuteMandelbrotFunction)
			RUN_TEST(FractalCommandDelegate, RunCFCFCFMove)
			RUN_TEST(FractalCommandDelegate, RunCFCFCFAdd)
			RUN_TEST(FractalCommandDelegate, RunCFCFCFMult)
			return 0;
	}