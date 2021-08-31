#pragma once
#include "EscapeTimeFractal.h"
#include "glm/vec2.hpp"

class ComplexFractal
{
public:
    double blendPower = 1;
    double lengthLimit = 100;
    ComplexFractal();
    ComplexFractal(int iterations);
    ComplexFractal(int iterations, CF_Float minDeviation, int deviationCycle = 100, bool debugDeviations = false);
    void SetFunction(ComplexFloat(*func)(ComplexFloat input, ComplexFloat previousValue, ComplexFloat extraValue));
    void SetStartingFunction(ComplexFloat(*func)(ComplexFloat input, ComplexFloat extraValue));
    double CalculateEscapeTime(CF_Float x, CF_Float y, ComplexFloat extraValue, glm::vec2 &uv);
protected:
    CF_Float minDeviationSqr = 0;
    int deviationCycle = 100;
    int iterations = 0;
    bool debugDeviations = false;
    ComplexFloat(*ComplexFunction)(ComplexFloat input, ComplexFloat previousValue, ComplexFloat extraValue) = 0;
    ComplexFloat(*StartingValueFunction)(ComplexFloat input, ComplexFloat extraValue) = 0;
};
