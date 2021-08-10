#pragma once
#include "EscapeTimeFractal.h"
class JuliaFractal :
    public EscapeTimeFractal
{
public:
    float CalculateEscapeTime(CF_Float x, CF_Float y);
protected:
    ComplexFloat startingValue;
};

