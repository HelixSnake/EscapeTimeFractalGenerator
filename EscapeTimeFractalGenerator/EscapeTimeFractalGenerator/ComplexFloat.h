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
	CF_Float AbsoluteValue();
	CF_Float AbsoluteValueSqr();
	ComplexFloat Normalized();
	static CF_Float Angle(const ComplexFloat complexVector1, const ComplexFloat complexVector2);
	static CF_Float Dot(const ComplexFloat complexVector1, const ComplexFloat complexVector2);
	static ComplexFloat Abs(const ComplexFloat cf);
};

