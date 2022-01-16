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
	ComplexFloat operator+(const CF_Float other) const;
	ComplexFloat operator-(const CF_Float other) const;
	ComplexFloat operator-() const;
	void operator+=(const ComplexFloat other);
	void operator-=(const ComplexFloat other);
	ComplexFloat operator*(const ComplexFloat other) const;
	ComplexFloat operator*(const CF_Float other) const; 
	ComplexFloat operator/(const CF_Float other) const;
	ComplexFloat operator/(const ComplexFloat other) const; 
	static ComplexFloat Divide(const CF_Float first, const ComplexFloat second);
	static ComplexFloat Subtract(const CF_Float first, const ComplexFloat second);
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
	static ComplexFloat Power(const ComplexFloat first, const CF_Float second);
	static ComplexFloat MultByi(const ComplexFloat first);

	static ComplexFloat Sqrt(ComplexFloat cfloat);
	static ComplexFloat Sin(ComplexFloat cfloat);
	static ComplexFloat Cos(ComplexFloat cfloat);
	static ComplexFloat Sinh(ComplexFloat cfloat);
	static ComplexFloat Cosh(ComplexFloat cfloat);
	static ComplexFloat Ln(ComplexFloat cfloat);
	
	static ComplexFloat Asin(ComplexFloat cfloat); //TODO: Test function
	/*
	static ComplexFloat Acos(ComplexFloat cfloat);
	static ComplexFloat Atan(ComplexFloat cfloat);
	static ComplexFloat Asinh(ComplexFloat cfloat);
	static ComplexFloat Acosh(ComplexFloat cfloat);
	static ComplexFloat Atanh(ComplexFloat cfloat);*/
};

