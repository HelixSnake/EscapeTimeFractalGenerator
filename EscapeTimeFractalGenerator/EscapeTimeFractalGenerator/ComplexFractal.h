#pragma once
#include "EscapeTimeFractal.h"
class ComplexFractal :
    public EscapeTimeFractal
{
public:
    float blendPower = 0.5;
    float lengthLimit = 100;
    ComplexFractal();
    ComplexFractal(int iterations);
    void SetFunction(ComplexFloat(*func)(ComplexFloat input, ComplexFloat previousValue));
    void SetStartingFunction(ComplexFloat(*func)(ComplexFloat input));
    float CalculateEscapeTime(CF_Float x, CF_Float y) override;
protected:
    ComplexFloat(*ComplexFunction)(ComplexFloat input, ComplexFloat previousValue) = 0;
    ComplexFloat(*StartingValueFunction)(ComplexFloat input) = 0;
};
