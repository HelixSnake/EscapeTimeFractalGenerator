#pragma once

#define CF_Float long double
struct ComplexFloat
{
	CF_Float real;
	CF_Float imaginary;

	ComplexFloat();
	ComplexFloat(CF_Float real, CF_Float imaginary);
	ComplexFloat operator+(const ComplexFloat other) const;
	ComplexFloat operator-(const ComplexFloat other) const;
	ComplexFloat operator*(const ComplexFloat other) const;
	ComplexFloat operator*(const CF_Float other) const;
	bool operator==(const ComplexFloat other) const { return real == other.real && imaginary == other.imaginary; }
	CF_Float AbsoluteValue() const;
	CF_Float AbsoluteValueSqr() const;
	ComplexFloat Normalized() const;
	static CF_Float Angle(const ComplexFloat complexVector1, const ComplexFloat complexVector2);
	static CF_Float Dot(const ComplexFloat complexVector1, const ComplexFloat complexVector2);
	ComplexFloat Abs() const;
	CF_Float Arg() const;
	ComplexFloat Reciprocal() const;
	ComplexFloat Power(int power) const;
	static ComplexFloat Power(const CF_Float first, const ComplexFloat second);
	static ComplexFloat Power(const ComplexFloat first, const ComplexFloat second);
};

