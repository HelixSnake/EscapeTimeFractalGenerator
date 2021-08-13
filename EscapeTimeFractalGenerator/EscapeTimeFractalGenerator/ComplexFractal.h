#pragma once
#include "EscapeTimeFractal.h"
class ComplexFractal
{
public:
    float blendPower = 1;
    float lengthLimit = 100;
    ComplexFractal();
    ComplexFractal(int iterations);
    void SetFunction(ComplexFloat(*func)(ComplexFloat input, ComplexFloat previousValue, float time));
    void SetStartingFunction(ComplexFloat(*func)(ComplexFloat input, float time));
    float CalculateEscapeTime(CF_Float x, CF_Float y, float time);
protected:
    int iterations = 0;
    ComplexFloat(*ComplexFunction)(ComplexFloat input, ComplexFloat previousValue, float time) = 0;
    ComplexFloat(*StartingValueFunction)(ComplexFloat input, float time) = 0;
};
