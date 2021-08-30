#include "ComplexFloat.h"
#include <math.h>

ComplexFloat::ComplexFloat()
{
	real = 0;
	imaginary = 0;
}
ComplexFloat::ComplexFloat(CF_Float real, CF_Float imaginary)
{
	this->real = real;
	this->imaginary = imaginary;
}
ComplexFloat ComplexFloat::operator+(ComplexFloat other)
{
	ComplexFloat output;
	output.real = other.real + this->real;
	output.imaginary = other.imaginary + this->imaginary;
	return output;
}
ComplexFloat ComplexFloat::operator-(ComplexFloat other)
{
	ComplexFloat output;
	output.real = other.real - this->real;
	output.imaginary = other.imaginary - this->imaginary;
	return output;
}
ComplexFloat ComplexFloat::operator*(ComplexFloat other)
{
	ComplexFloat output;
	output.real = other.real * this->real - other.imaginary * this->imaginary;
	output.imaginary = other.imaginary * this->real + this->imaginary * other.real;
	return output;
}

CF_Float ComplexFloat::AbsoluteValue()
{
	return (CF_Float)sqrt(this->real * this->real + this->imaginary * this->imaginary);
}
CF_Float ComplexFloat::AbsoluteValueSqr()
{
	return this->real * this->real + this->imaginary * this->imaginary;
}

ComplexFloat ComplexFloat::Normalized()
{
	CF_Float abs = this->AbsoluteValue();
	if (abs == 0)
	{
		return ComplexFloat(0, 0);
	}
	return ComplexFloat(this->real / abs, this->imaginary / abs);
}

CF_Float ComplexFloat::Angle(const ComplexFloat complexVector1, const ComplexFloat complexVector2) {
	double x1 = complexVector1.real;
	double x2 = complexVector2.real;
	double y1 = complexVector1.imaginary;
	double y2 = complexVector2.imaginary;
	return atan2(x1 * y2 - x2 * y1, x1 * x2 + y1 * y2);
}

CF_Float ComplexFloat::Dot(const ComplexFloat complexVector1, const ComplexFloat complexVector2) {
	double x1 = complexVector1.real;
	double x2 = complexVector2.real;
	double y1 = complexVector1.imaginary;
	double y2 = complexVector2.imaginary;
	return x1 * x2 + y1 * y2;
}