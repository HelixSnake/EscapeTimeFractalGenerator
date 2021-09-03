#pragma once
#include <thread>
#include <chrono>
#include <future>
#include <mutex>
#define GLEW_STATIC
#include <GL/glew.h>
#include "ComplexFloat.h"

using namespace std::chrono;

class FractalInterpreter // Purpose: take an array of CF_Floats from FractalDrawer and change it to an array of colors using a ramp function
{
public:
	FractalInterpreter();
	~FractalInterpreter();
	void SetRampTexture(GLuint textureID);
	void CreateOrUpdateBuffers(int width, int height);
	CF_Float* GetValueBufferStart();
	bool Draw(bool startDrawing);
	const float* GetColors(int &width, int &height);
	float GetProgress();
	float GetInterpreterTime() { return interpreterTime; } // Gets the last time taken by the thread, which should be fairly consistent, to help with smoothZooming
	bool IsBusy();
	float period = 1;
	float offset = 0;
protected:
	bool busyDrawing = false;
	bool drawNext = false;
	bool Draw_Threaded();
	void ResizeOnSizeChanged(int width, int height);
	CF_Float* valueBuffer = nullptr;
	float* colorBuffer = nullptr;
	float* finishedColorBuffer = nullptr;
	float* rampColors = nullptr;
	int rampColorsLength = 0;
	int bufferWidth = 0;
	int bufferHeight = 0;
	std::future<bool> interpreterThread;
	std::atomic<int> threadProgress;
	std::mutex interpreterMutex;
	steady_clock::time_point interpreterTimeStart;
	float interpreterTime = 0;
};

