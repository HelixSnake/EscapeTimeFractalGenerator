#include <glm/common.hpp>
#include "FractalInterpreter.h"

FractalInterpreter::FractalInterpreter()
{
}
FractalInterpreter::~FractalInterpreter()
{
}


void FractalInterpreter::CreateOrUpdateBuffers(int width, int height) {
	if (valueBuffer == nullptr || colorBuffer == nullptr || bufferWidth != width || bufferHeight != height)
	{
		ResizeOnSizeChanged(width, height);
	}
}

CF_Float* FractalInterpreter::GetValueBufferStart()
{
	return valueBuffer;
}

const float* FractalInterpreter::GetColors(int& width, int& height)
{
	height = bufferHeight;
	width = bufferWidth;
	return colorBuffer;
}

void FractalInterpreter::Draw()
{
	//TODO: Implement ramp texture sampling
	for (int i = 0; i < bufferWidth; i++)
	{
		for (int j = 0; j < bufferHeight; j++) 
		{
			int valueBufferPos = j * bufferWidth + i;
			int colorBufferPos = valueBufferPos * 3;
			float colorValue = glm::fract(valueBuffer[valueBufferPos]);
			colorBuffer[colorBufferPos] = colorValue;
			colorBuffer[colorBufferPos + 1] = colorValue;
			colorBuffer[colorBufferPos + 2] = colorValue;
		}
	}

}

void FractalInterpreter::ResizeOnSizeChanged(int width, int height)
{
	if (valueBuffer != nullptr) delete[] valueBuffer;
	if (colorBuffer != nullptr) delete[] colorBuffer;
	valueBuffer = new CF_Float[width * height];
	colorBuffer = new float[width * height * 3];
	bufferWidth = width;
	bufferHeight = height;
}