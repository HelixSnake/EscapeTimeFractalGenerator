#pragma once

#define CF_Float float
struct ComplexFloat
{
	CF_Float real;
	CF_Float imaginary;

	ComplexFloat();
	ComplexFloat(CF_Float real, CF_Float imaginary);
	ComplexFloat operator+(const ComplexFloat other);
	ComplexFloat operator-(const ComplexFloat other);
	ComplexFloat operator*(const ComplexFloat other);
	CF_Float AbsoluteValue();
	CF_Float AbsoluteValueSqr();
};

