#pragma once
#include <iostream>
#include <thread>
#include <chrono>
#include <future>
//GLEW
//#define GLEW_STATIC //Extracting openGL functionality to Quad Drawer class to remove code smell
//#include <GL/glew.h>

//GLFW
//#include <GLFW/glfw3.h>

//GLM
#include <glm/common.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "ComplexFloat.h"
#include "ZoomTransform.h"
#include "FractalDictionary.h"

using namespace std::chrono;

const int NUM_FRACTAL_DRAW_THREADS = std::thread::hardware_concurrency();

class FractalDrawer
{
public:
	FractalDrawer(int width, int height);
	~FractalDrawer();
	//void SetRampTexture(GLuint textureID);
	void Resize(int width, int height, double sizeMult);
	void SetIterations(int iterations);
	void SetMinDeviation(double minDeviation);
	void SetDeviationCycles(int deviationCycles, bool debugDeviations);
	void SetLengthLimit(double lengthLimit);
	void SetFractalType(FractalDictionary::FractalType fractal);
	FractalDictionary::FractalType GetFractalType();
	float GetProgress() { return drawingProgress; }
	bool ShouldRestartInterpreter() { return shouldRestartInterpreter; }
	bool IsBusy() { return isBusy; }
	ZoomTransform GetRenderedZoom() { return renderedZoom; }
	int GetMipLevel();
	bool Draw(bool update, ZoomTransform transform, ComplexFloat* extraValues); //Returns true when you should continue drawing afterwards. LiveUpdate will result in true during rendering, unless zooming in or out.
	void GetBufferDimensions(int& bufferWidth, int& bufferHeight);
	void CopyBuffer(CF_Float* dest, size_t bufferSize);
protected:

	std::atomic<CF_Float>* pixelBuffer = nullptr;

	FractalDictionary::FractalType currentFractal;
	float drawingProgress = 0;

	std::atomic_int iterations = 40;
	std::atomic<double> minDeviation = 0;
	std::atomic<int> deviationCycles = 100;
	std::atomic<bool> debugDeviations = false;
	std::atomic<double> lengthLimit = 10;

	int pixelBufferHeight = 0;
	int pixelBufferWidth = 0;
	float upScale = 1;
	bool shouldRestartInterpreter = false;
	bool isBusy = false;
	ZoomTransform renderedZoom;

	std::atomic_bool haltDrawingThread = false;
	std::future<bool> drawFractalThread;

	std::future<bool> *drawFractalThreads;
	std::atomic<int> *threadProgress;
    std::mutex *Mutexes;
	std::future_status* drawingStatus;

	static void SetPixel(std::atomic<CF_Float>* pixelBuffer, int pixelBufferWidth, int pixelBufferHeight, int x, int y, CF_Float value);
	//static bool DrawFractal(float* pixelBuffer, int pixelBufferWidth, int pixelBufferHeight, const float* rampColors, int rampColorsWidth, glm::vec3 transform, float time, std::atomic_bool &halt);
	bool DrawFractalChunk(int index, CF_Float tfx, CF_Float tfy, CF_Float tfscale, ComplexFloat* extraValues);
	void LockAllMutexes(bool haltDrawing = true);
	void UnlockAllMutexes();
};

