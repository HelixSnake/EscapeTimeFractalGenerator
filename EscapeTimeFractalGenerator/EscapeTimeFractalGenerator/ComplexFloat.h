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
	ComplexFloat operator*(const CF_Float other);
	CF_Float AbsoluteValue();
	CF_Float AbsoluteValueSqr();
	ComplexFloat Normalized();
	static CF_Float Angle(ComplexFloat complexVector1, ComplexFloat complexVector2);
	static CF_Float Dot(ComplexFloat complexVector1, ComplexFloat complexVector2);
	ComplexFloat Abs();
	ComplexFloat Power(int power);
};

