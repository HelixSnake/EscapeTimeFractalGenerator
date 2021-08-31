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
	void Draw();
	const float* GetColors(int &width, int &height);

	float period = 1;
	float offset = 0;
protected:
	bool Draw_Threaded();
	void ResizeOnSizeChanged(int width, int height);
	CF_Float* valueBuffer = nullptr;
	float* colorBuffer = nullptr;
	float* rampColors = nullptr;
	int rampColorsLength = 0;
	int bufferWidth = 0;
	int bufferHeight = 0;
	std::future<bool> interpreterThread;
	std::mutex interpreterMutex;
};

