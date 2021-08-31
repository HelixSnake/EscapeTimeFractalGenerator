#pragma once
#include "ComplexFloat.h"
class FractalInterpreter // Purpose: take an array of CF_Floats from FractalDrawer and change it to an array of colors using a ramp function
{
public:
	FractalInterpreter();
	~FractalInterpreter();
	void CreateOrUpdateBuffers(int width, int height);
	CF_Float* GetValueBufferStart();
	void Draw();
	const float* GetColors(int &width, int &height);
protected:
	void ResizeOnSizeChanged(int width, int height);
	CF_Float* valueBuffer = nullptr;
	float* colorBuffer = nullptr;
	int bufferWidth = 0;
	int bufferHeight = 0;
};

