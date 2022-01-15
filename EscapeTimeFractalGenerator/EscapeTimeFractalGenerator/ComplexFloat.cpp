#include "ComplexFloat.h"
#include <math.h>
#include <glm/common.hpp>

const long double E_CONSTANT = 2.71828182845904523536028747135266249775724709369995;
const long double PI_CONSTANT = 3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117068; // is this enough digits?
const long double SQRT2 = 1.4142135623730950488016887242096980785696718753769480731766797379907324784621;

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
	output.real = this->real - other.real;
	output.imaginary = this->imaginary - other.imaginary;
	return output;
}
ComplexFloat ComplexFloat::operator-() const
{
	ComplexFloat output;
	output.real =  -this->real;
	output.imaginary = -this->imaginary;
	return output;
}
ComplexFloat ComplexFloat::operator+(const CF_Float other) const
{
	return ComplexFloat(this->real + other, this->imaginary);
}
ComplexFloat ComplexFloat::operator-(const CF_Float other) const
{
	return ComplexFloat(this->real - other, this->imaginary);
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
ComplexFloat ComplexFloat::operator/(const CF_Float other) const
{
	return ComplexFloat(this->real / other, this->imaginary / other);
}
ComplexFloat ComplexFloat::operator/(const ComplexFloat other) const
{
	ComplexFloat output;
	output.real = this->real* other.real + this->imaginary * other.imaginary;
	output.imaginary = this->imaginary * other.real - this->real*other.imaginary;
	return output / (other.real * other.real + other.imaginary * other.imaginary);
}

ComplexFloat ComplexFloat::Divide(const CF_Float first, const ComplexFloat second)
{
	ComplexFloat output;
	output.real = first * second.real;
	output.imaginary = - first * second.imaginary;
	return output / (second.real * second.real + second.imaginary * second.imaginary);
}
ComplexFloat ComplexFloat::Subtract(const CF_Float first, const ComplexFloat second)
{
	return ComplexFloat(first - second.real, -second.imaginary);
}

CF_Float ComplexFloat::AbsoluteValue() const
{
	return (CF_Float)sqrtl(this->real * this->real + this->imaginary * this->imaginary);
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
	CF_Float x1 = complexVector1.real;
	CF_Float x2 = complexVector2.real;
	CF_Float y1 = complexVector1.imaginary;
	CF_Float y2 = complexVector2.imaginary;
	return atan2l(x1 * y2 - x2 * y1, x1 * x2 + y1 * y2);
}

CF_Float ComplexFloat::Dot(const ComplexFloat complexVector1, const ComplexFloat complexVector2) {
	CF_Float x1 = complexVector1.real;
	CF_Float x2 = complexVector2.real;
	CF_Float y1 = complexVector1.imaginary;
	CF_Float y2 = complexVector2.imaginary;
	return x1 * x2 + y1 * y2;
}
ComplexFloat ComplexFloat::Abs() const
{
	return ComplexFloat(glm::abs(this->real), glm::abs(this->imaginary));
}

CF_Float ComplexFloat::Arg() const
{
	return atan2l(imaginary, real);
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
	if (power < 0 || power > 20)
	{
		return ComplexFloat::Power(*this, CF_Float(power));
	}
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
	return ComplexFloat(cosl(clna), sinl(clna)) * powl(first, second.real);
}

ComplexFloat ComplexFloat::Power(const ComplexFloat first, const ComplexFloat second)
{
	if (first.real == 0 && first.imaginary == 0) //will cause multiple math errors if this is true; just return 0 + 0i
	{
		return first;
	}
	ComplexFloat step1 = second * logl(first.AbsoluteValue());
	ComplexFloat step2 = step1 + ComplexFloat(0, first.Arg()) * second;
	return ComplexFloat(cosl(step2.imaginary), sinl(step2.imaginary)) * powl(E_CONSTANT, step2.real);
}
ComplexFloat ComplexFloat::Power(const ComplexFloat first, const CF_Float second)
{
	if (first.real == 0 && first.imaginary == 0) //will cause multiple math errors if this is true; just return 0 + 0i
	{
		return first;
	}
	CF_Float r = first.AbsoluteValue();
	CF_Float nt = second*first.Arg();
	return ComplexFloat(cosl(nt), sinl(nt)) * powl(r, second);
}

ComplexFloat ComplexFloat::MultByi(const ComplexFloat first)
{
	return ComplexFloat(-first.imaginary, first.real);
}
ComplexFloat ComplexFloat::Sqrt(ComplexFloat cfloat)
{
	CF_Float r = cfloat.AbsoluteValue();
	CF_Float ssign = cfloat.imaginary < 0 ? (CF_Float )-1 : (CF_Float)1;
	return ComplexFloat(sqrtl(r + cfloat.real), ssign * sqrtl(r - cfloat.real)) * SQRT2 * 0.5;
}

ComplexFloat ComplexFloat::Sin(ComplexFloat cfloat)
{
	return ComplexFloat(sinl(cfloat.real) * coshl(cfloat.imaginary), cosl(cfloat.real) * sinhl(cfloat.imaginary));
}
ComplexFloat ComplexFloat::Cos(ComplexFloat cfloat)
{
	return ComplexFloat(cosl(cfloat.real) * coshl(cfloat.imaginary), -sinl(cfloat.real) * sinhl(cfloat.imaginary));
}
ComplexFloat ComplexFloat::Sinh(ComplexFloat cfloat) 
{
	return ComplexFloat(sinhl(cfloat.real) * cosl(cfloat.imaginary), coshl(cfloat.real) * sinl(cfloat.imaginary));
}
ComplexFloat ComplexFloat::Cosh(ComplexFloat cfloat)
{
	return ComplexFloat(coshl(cfloat.real) * cosl(cfloat.imaginary), sinhl(cfloat.real) * sinl(cfloat.imaginary));
}
ComplexFloat ComplexFloat::Ln(ComplexFloat cfloat)
{
	return ComplexFloat(logl(cfloat.AbsoluteValue()), cfloat.Arg());
}
ComplexFloat ComplexFloat::Asin(ComplexFloat cfloat)
{
	ComplexFloat part1 = ComplexFloat::Sqrt(ComplexFloat::Subtract(1, cfloat * cfloat));
	ComplexFloat part2 = ComplexFloat::Ln(part1 + ComplexFloat::MultByi(cfloat));
	return -ComplexFloat::MultByi(part2);
}/*
ComplexFloat ComplexFloat::Acos(ComplexFloat cfloat)
{}
ComplexFloat ComplexFloat::Atan(ComplexFloat cfloat)
{}
ComplexFloat ComplexFloat::Asinh(ComplexFloat cfloat)
{}
ComplexFloat ComplexFloat::Acosh(ComplexFloat cfloat)
{}
ComplexFloat ComplexFloat::Atanh(ComplexFloat cfloat)
{

}*/