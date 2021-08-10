#pragma once
#include "EscapeTimeFractal.h"
class JuliaFractal :
    public EscapeTimeFractal
{
public:
    JuliaFractal();
    JuliaFractal(ComplexFloat startingValue);
    JuliaFractal(ComplexFloat startingValue, int iterations);
    float CalculateEscapeTime(CF_Float x, CF_Float y) override;
protected:
    ComplexFloat startingValue;
};

