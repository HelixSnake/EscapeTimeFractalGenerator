#pragma once

#define CF_Float long double
struct ComplexFloat
{
	CF_Float real;
	CF_Float imaginary;

	ComplexFloat();
	ComplexFloat(CF_Float real, CF_Float imaginary);
	ComplexFloat operator+(const ComplexFloat other);
	ComplexFloat operator-(const ComplexFloat other);
	ComplexFloat operator*(const ComplexFloat other);
};

