#pragma once
#include "EscapeTimeFractal.h"
class ComplexFractal
{
public:
    float blendPower = 1;
    float lengthLimit = 100;
    ComplexFractal();
    ComplexFractal(int iterations);
    ComplexFractal(int iterations, CF_Float minDeviation);
    void SetFunction(ComplexFloat(*func)(ComplexFloat input, ComplexFloat previousValue, ComplexFloat extraValue));
    void SetStartingFunction(ComplexFloat(*func)(ComplexFloat input, ComplexFloat extraValue));
    float CalculateEscapeTime(CF_Float x, CF_Float y, ComplexFloat extraValue);
protected:
    CF_Float minDeviationSqr = 0;
    int iterations = 0;
    ComplexFloat(*ComplexFunction)(ComplexFloat input, ComplexFloat previousValue, ComplexFloat extraValue) = 0;
    ComplexFloat(*StartingValueFunction)(ComplexFloat input, ComplexFloat extraValue) = 0;
};
