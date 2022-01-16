#include <glm/common.hpp>
#include <glm/vec3.hpp>
#include "FractalInterpreter.h"
#include <thread>
#include <string>
#include <iostream>

FractalInterpreter::FractalInterpreter(int width, int height)
{
	CreateOrUpdateBuffers(width, height);
	busyDrawing = false;
	interpreterTimeStart = high_resolution_clock::now();
}
FractalInterpreter::~FractalInterpreter()
{
	const std::lock_guard<std::mutex> lock(interpreterMutex);
	if (rampColors != nullptr) delete[] rampColors;
	if (colorBuffer != nullptr) delete[] colorBuffer;
	if (finishedColorBuffer != nullptr) delete[] finishedColorBuffer;
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
		rampColors = new unsigned char[rampColorsLength * 4];
		glGetTextureSubImage(textureID, 0, 0, 0, 0, rampColorsLength, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, rampColorsLength * 4 * sizeof(unsigned char), rampColors);
	}
}

void FractalInterpreter::CreateOrUpdateBuffers(int width, int height) {
	if (colorBuffer == nullptr || finishedColorBuffer == nullptr || bufferWidth != width || bufferHeight != height)
	{
		ResizeOnSizeChanged(width, height);
	}
}

const unsigned char* FractalInterpreter::GetColors(int& width, int& height)
{
	//const std::lock_guard<std::mutex> lock(interpreterMutex);
	height = bufferHeight;
	width = bufferWidth;
	return finishedColorBuffer;
}

float FractalInterpreter::GetProgress()
{
	float progress = threadProgress.load();
	return progress / (bufferHeight * bufferWidth);
}

bool FractalInterpreter::IsBusy()
{
	return busyDrawing;
}

bool FractalInterpreter::Draw(bool startDrawing, ThreadSafeBuffer<ComplexFloat>* buffer)
{
	bool finishedThisFrame = false;
	if (!interpreterThread.valid() && (startDrawing || drawNext))
	{
		busyDrawing = true;
		drawNext = false;
		threadProgress = 0;
		interpreterTimeStart = high_resolution_clock::now(); // time how long this thread takes (should always be the same for the same resolution) to help the smoothzoomer
		interpreterThread = std::async(std::launch::async, &FractalInterpreter::Draw_Threaded, this, buffer);
	}
	else if (startDrawing)
	{
		drawNext = true;
	}

	if (interpreterThread.valid()) {
		std::future_status drawingStatus = interpreterThread.wait_for(std::chrono::seconds(0));
		if (drawingStatus == std::future_status::ready)
		{
			if (interpreterThread.get())
			{
				busyDrawing = false;
				interpreterMutex.lock();
				memcpy(finishedColorBuffer, colorBuffer, bufferHeight * bufferWidth * sizeof(unsigned char) * 4);
				interpreterMutex.unlock();
				finishedThisFrame = true;
				steady_clock::time_point currentTime = high_resolution_clock::now();
				interpreterTime = duration_cast<nanoseconds>(currentTime - interpreterTimeStart).count() / 1000000000.0;
			}
		}
	}
	return (finishedThisFrame);
}

bool FractalInterpreter::Draw_Threaded(ThreadSafeBuffer<ComplexFloat>* buffer)
{
	const std::lock_guard<std::mutex> lock(interpreterMutex);
	double oneDivPeriod = 1.0 / period;
	for (int y = 0; y < bufferHeight; y++)
	{
		for (int x = 0; x < bufferWidth; x++) 
		{
			int valueBufferPos = y * bufferWidth + x;
			int colorBufferPos = valueBufferPos * 4;
			double newValue = 0; 
			unsigned char color[4] = { 0, 0, 0, 1 };
			if ((*buffer)[valueBufferPos].real >= 0.0000000001)
			{
				//logarithmic function keeps period similar at all zoom levels
				newValue = glm::fract(log((*buffer)[valueBufferPos].real) * oneDivPeriod + offset);
				if (rampColors != nullptr)
				{
					int rampIndex = ((int)((double)rampColorsLength * (1 - newValue)));
					if (rampIndex > rampColorsLength - 1) rampIndex = rampColorsLength - 1;
					if (rampIndex < 0) rampIndex = 0;
					rampIndex *= 3;
					//glm::vec3 color = glm::vec3(UV.x, UV.y, 0); // UVs in case I decide to implement reading from textures
					color[0] = rampColors[rampIndex];
					color[1] = rampColors[rampIndex + 1]; 
					color[2] = rampColors[rampIndex + 2];
					color[3] = 255;
				}
			}
			//colorBuffer[colorBufferPos] = (unsigned char)(color.r);
			//colorBuffer[colorBufferPos + 1] = (unsigned char)(color.g);
			//colorBuffer[colorBufferPos + 2] = (unsigned char)(color.b);
			colorBuffer[colorBufferPos] = color[0];
			colorBuffer[colorBufferPos + 1] = color[1];
			colorBuffer[colorBufferPos + 2] = color[2];
			colorBuffer[colorBufferPos + 3] = color[3];
			threadProgress++;
		}
	}
	return true;
}

void FractalInterpreter::ResizeOnSizeChanged(int width, int height)
{
	const std::lock_guard<std::mutex> lock(interpreterMutex);
	if (colorBuffer != nullptr) delete[] colorBuffer;
	if (finishedColorBuffer != nullptr) delete[] finishedColorBuffer;
	colorBuffer = new unsigned char[width * height * 4];
	finishedColorBuffer = new unsigned char[width * height * 4];
	bufferWidth = width;
	bufferHeight = height;
}