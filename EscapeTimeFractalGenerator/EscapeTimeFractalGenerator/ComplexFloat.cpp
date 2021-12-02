#include "ComplexFloat.h"
#include <math.h>
#include <glm/common.hpp>

const long double E_CONSTANT = 2.71828182845904523536028747135266249775724709369995;

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
ComplexFloat ComplexFloat::operator+(const ComplexFloat other) const
{
	ComplexFloat output;
	output.real = other.real + this->real;
	output.imaginary = other.imaginary + this->imaginary;
	return output;
}
ComplexFloat ComplexFloat::operator-(const ComplexFloat other) const
{
	ComplexFloat output;
	output.real = other.real - this->real;
	output.imaginary = other.imaginary - this->imaginary;
	return output;
}
ComplexFloat ComplexFloat::operator*(const ComplexFloat other) const
{
	ComplexFloat output;
	output.real = other.real * this->real - other.imaginary * this->imaginary;
	output.imaginary = other.imaginary * this->real + this->imaginary * other.real;
	return output;
}
ComplexFloat ComplexFloat::operator*(const CF_Float other) const
{
	return ComplexFloat(this->real * other, this->imaginary * other);
}

CF_Float ComplexFloat::AbsoluteValue() const
{
	return (CF_Float)sqrt(this->real * this->real + this->imaginary * this->imaginary);
}
CF_Float ComplexFloat::AbsoluteValueSqr() const
{
	return this->real * this->real + this->imaginary * this->imaginary;
}

ComplexFloat ComplexFloat::Normalized() const
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
ComplexFloat ComplexFloat::Abs() const
{
	return ComplexFloat(glm::abs(this->real), glm::abs(this->imaginary));
}

CF_Float ComplexFloat::Arg() const
{
	return atan2(imaginary, real);
}

ComplexFloat ComplexFloat::Reciprocal() const
{
	if (real == 0 && imaginary == 0)
	{
		return *this;
	}
	return ComplexFloat(real, -imaginary) * (1 / AbsoluteValueSqr());
}
ComplexFloat ComplexFloat::Power(const int power) const
{
	ComplexFloat newValue = *this;
	for (int i = 1; i < power; i++)
	{
		newValue = newValue * *this;
	}
	return newValue;
}

ComplexFloat ComplexFloat::Power(const CF_Float first, const ComplexFloat second)
{
	if (first == 0) return ComplexFloat(0,0); //math errors will occur if we try to do the math normally when first is 0
	CF_Float clna = second.imaginary * log(first);
	return ComplexFloat(cos(clna), sin(clna)) * pow(first, second.real);
}

ComplexFloat ComplexFloat::Power(const ComplexFloat first, const ComplexFloat second)
{
	if (first.real == 0 && first.imaginary == 0) //will cause multiple math errors if this is true; just return 0 + 0i
	{
		return first;
	}
	ComplexFloat exponent1 = second * log(first.AbsoluteValue());
	ComplexFloat exponent2 = ComplexFloat(0, first.Arg()) * second;
	return ComplexFloat::Power(E_CONSTANT, exponent1 + exponent2);
}