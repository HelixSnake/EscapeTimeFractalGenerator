#include <glm/common.hpp>
#include <glm/vec3.hpp>
#include "FractalInterpreter.h"
#include <thread>

FractalInterpreter::FractalInterpreter()
{
}
FractalInterpreter::~FractalInterpreter()
{
	const std::lock_guard<std::mutex> lock(interpreterMutex);
	if (rampColors != nullptr)
	{
		delete[] rampColors;
	}
}

void FractalInterpreter::SetRampTexture(GLuint textureID)
{
	const std::lock_guard<std::mutex> lock(interpreterMutex);
	// If we are running this a second time, make sure to delete our rampColors array to prevent memory leaks
	if (rampColors != nullptr)
	{
		delete[] rampColors;
	}
	// Build Ramp Color Array
	if (textureID != 0)
	{
		glBindTexture(GL_TEXTURE_2D, textureID);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &rampColorsLength);
		rampColors = new float[rampColorsLength * 3];
		glGetTextureSubImage(textureID, 0, 0, 0, 0, rampColorsLength, 1, 1, GL_RGB, GL_FLOAT, rampColorsLength * 3 * sizeof(float), rampColors);
	}
}

void FractalInterpreter::CreateOrUpdateBuffers(int width, int height) {
	if (valueBuffer == nullptr || colorBuffer == nullptr || bufferWidth != width || bufferHeight != height)
	{
		ResizeOnSizeChanged(width, height);
	}
}

CF_Float* FractalInterpreter::GetValueBufferStart()
{
	//const std::lock_guard<std::mutex> lock(interpreterMutex);
	return valueBuffer;
}

const float* FractalInterpreter::GetColors(int& width, int& height)
{
	//const std::lock_guard<std::mutex> lock(interpreterMutex);
	height = bufferHeight;
	width = bufferWidth;
	return colorBuffer;
}

void FractalInterpreter::Draw()
{
	if (!interpreterThread.valid())
	{
		interpreterThread = std::async(std::launch::async, &FractalInterpreter::Draw_Threaded, this);
	}
	std::future_status drawingStatus = interpreterThread.wait_for(std::chrono::seconds(0));
	if (drawingStatus == std::future_status::ready)
	{
		interpreterThread.get();
	}
}
bool FractalInterpreter::Draw_Threaded()
{
	const std::lock_guard<std::mutex> lock(interpreterMutex);
	for (int i = 0; i < bufferWidth; i++)
	{
		for (int j = 0; j < bufferHeight; j++) 
		{
			int valueBufferPos = j * bufferWidth + i;
			int colorBufferPos = valueBufferPos * 3;
			double newValue = 0; 
			glm::vec3 color = glm::vec3(0, 0, 0);
			if (valueBuffer[valueBufferPos] != 0)
			{
				//logarithmic function keeps period similar at all zoom levels
				newValue = glm::fract(log(valueBuffer[valueBufferPos]) / period + offset);
				color = glm::vec3(newValue, newValue, newValue);
				if (rampColors != nullptr)
				{
					int rampIndex = ((int)((double)rampColorsLength * (1 - newValue)));
					if (rampIndex > rampColorsLength - 1) rampIndex = rampColorsLength - 1;
					if (rampIndex < 0) rampIndex = 0;
					rampIndex *= 3;
					//glm::vec3 color = glm::vec3(UV.x, UV.y, 0); // UVs in case I decide to implement reading from textures
					color = glm::vec3(rampColors[rampIndex], rampColors[rampIndex + 1], rampColors[rampIndex + 2]);
				}
			}
			colorBuffer[colorBufferPos] = color.r;
			colorBuffer[colorBufferPos + 1] = color.g;
			colorBuffer[colorBufferPos + 2] = color.b;
		}
	}
	return true;
}

void FractalInterpreter::ResizeOnSizeChanged(int width, int height)
{
	const std::lock_guard<std::mutex> lock(interpreterMutex);
	if (valueBuffer != nullptr) delete[] valueBuffer;
	if (colorBuffer != nullptr) delete[] colorBuffer;
	valueBuffer = new CF_Float[width * height];
	colorBuffer = new float[width * height * 3];
	bufferWidth = width;
	bufferHeight = height;
}