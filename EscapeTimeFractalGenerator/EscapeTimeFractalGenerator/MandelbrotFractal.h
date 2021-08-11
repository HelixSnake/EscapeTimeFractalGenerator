#pragma once
#include "EscapeTimeFractal.h"
class MandelbrotFractal :
    public EscapeTimeFractal
{
public:
    MandelbrotFractal();
    MandelbrotFractal(ComplexFloat startingValue);
    MandelbrotFractal(ComplexFloat startingValue, int iterations);
    float CalculateEscapeTime(CF_Float x, CF_Float y) override;
protected:
    ComplexFloat startingValue;
};

