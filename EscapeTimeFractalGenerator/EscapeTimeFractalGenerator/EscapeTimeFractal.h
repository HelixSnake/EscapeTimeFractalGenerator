#pragma once
#include "ComplexFloat.h"

class EscapeTimeFractal
{
public:
	virtual float CalculateEscapeTime(CF_Float x, CF_Float y) = 0;
protected:
	int iterations;
};

