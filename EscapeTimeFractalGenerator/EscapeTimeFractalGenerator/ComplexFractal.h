#pragma once
#include "EscapeTimeFractal.h"
#include "glm/vec2.hpp"
#include "FractalCommandListExecutor.h"

class ComplexFractal
{
public:
    typedef ComplexFloat(*RecursiveFunction)(ComplexFloat input, ComplexFloat previousValue, ComplexFloat* extraValues, int power);
    typedef ComplexFloat(*StartingValueFunction)(ComplexFloat input, ComplexFloat* extraValues, int power);
    double blendPower = 1;
    double lengthLimit = 100;
    ComplexFractal();
    ComplexFractal(int iterations);
    ComplexFractal(int iterations, CF_Float minDeviation, int deviationCycle = 100, bool debugDeviations = false);
    void SetFunction(RecursiveFunction func);
    void SetStartingFunction(StartingValueFunction func);
    double CalculateEscapeTime(CF_Float x, CF_Float y, ComplexFloat* extraValues, CF_Float power);
    double CalculateEscapeTime(FractalCommandListExecutor &startingFunction, FractalCommandListExecutor &recursiveFunction, CF_Float power);
protected:
    CF_Float minDeviation = 0;
    int deviationCycle = 100;
    int iterations = 0;
    bool debugDeviations = false;
    RecursiveFunction recursiveFunction = 0;
    StartingValueFunction startingValueFunction = 0;
};
