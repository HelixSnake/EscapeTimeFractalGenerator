#pragma once
#include <thread>
#include <chrono>
#include <future>
#include <mutex>
#define GLEW_STATIC
#include <GL/glew.h>
#include "ComplexFloat.h"
class FractalInterpreter // Purpose: take an array of CF_Floats from FractalDrawer and change it to an array of colors using a ramp function
{
public:
	FractalInterpreter();
	~FractalInterpreter();
	void SetRampTexture(GLuint textureID);
	void CreateOrUpdateBuffers(int width, int height);
	CF_Float* GetValueBufferStart();
	bool Draw(bool startDrawing, bool shouldRestart);
	const float* GetColors(int &width, int &height);
	float GetProgress();
	bool IsBusy();
	float period = 1;
	float offset = 0;
protected:
	std::atomic_bool haltThread = false;
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
};

