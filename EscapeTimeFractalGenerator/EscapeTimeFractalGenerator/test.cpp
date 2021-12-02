#include "test.h"
#include "FractalCommandList.h"

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

BEGIN_TEST(FractalCommandList, ArrayConstruction)
	{
		unsigned int ints[5] = { 2, 4, 5, 7, 8 };
		FractalCommandList fcl(0, 0, 5, ints);
		for (int i = 0; i < 5; i++)
		{
			EXPECT_EQ(ints[i], fcl.GetCommandList()[i])
		}
	}END_TEST

int RunAllUnitTests()
	{
		RUN_TEST(AddProps, Commutative)
			RUN_TEST(FractalCommandList, ArrayConstruction)
			return 0;
	}