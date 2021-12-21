#include "HammerslySequence.h"

unsigned int InverseInt(unsigned int input) // reverse the int's binary
{
	unsigned int result = 0;
	const int length = sizeof(unsigned int) * 8;
	for (int i = 0; i < length / 2; i++)
	{
		unsigned int bitmask = 1 << i;
		result |= bitmask & (input >> (length - i * 2 - 1));
	}
	for (int i = length / 2; i < length; i++)
	{
		unsigned int bitmask = 1 << i;
		result |= bitmask & (input << (i * 2 - length + 1));
	}
	return result;
}
unsigned int SlideInt(unsigned int input, unsigned int bits)
{
	const int length = sizeof(unsigned int) * 8;
	return input << (length - bits);
}

double HammerslySequence::GetOffsetX(unsigned int input)
{
	return (long double)(InverseInt(input)) / (unsigned int)(-1) - 0.5;
}
double HammerslySequence::GetOffsetY(unsigned int input)
{
	return (long double)(SlideInt(input, 4)) / (unsigned int)(-1) - 0.5;
}