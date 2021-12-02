#include "test.h"

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

int RunAllUnitTests()
	{
		RUN_TEST(AddProps, Commutative)
			return 0;
	}