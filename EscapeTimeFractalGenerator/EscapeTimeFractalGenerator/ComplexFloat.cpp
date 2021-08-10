#include "ComplexFloat.h"
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
ComplexFloat ComplexFloat::operator+(const ComplexFloat other)
{
	ComplexFloat output;
	output.real = other.real + this->real;
	output.imaginary = other.imaginary + this->imaginary;
	return output;
}
ComplexFloat ComplexFloat::operator-(const ComplexFloat other)
{
	ComplexFloat output;
	output.real = other.real - this->real;
	output.imaginary = other.imaginary - this->imaginary;
	return output;
}
ComplexFloat ComplexFloat::operator*(const ComplexFloat other)
{
	ComplexFloat output;
	output.real = other.real * this->real;
	output.imaginary = other.imaginary * this->real + this->imaginary * other.real;
	output.real -= other.imaginary * this->imaginary;
	return output;
}