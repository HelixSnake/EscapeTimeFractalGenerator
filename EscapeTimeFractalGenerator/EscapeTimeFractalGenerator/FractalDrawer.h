#pragma once
#include <iostream>
#include <thread>
#include <chrono>
#include <future>
//GLEW
#define GLEW_STATIC
#include <GL/glew.h>

//GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm/vec3.hpp>
#include "ComplexFloat.h"

using namespace std::chrono;

const int NUM_FRACTAL_DRAW_THREADS = std::thread::hardware_concurrency();

enum class FractalType
{
	Mandelbrot,
	Julia
};
class FractalDrawer
{
public:
	bool enableAnimation = true;
	bool liveUpdate = true;
	FractalDrawer(int width, int height, GLFWwindow* window);
	~FractalDrawer();
	void SetRampTexture(GLuint textureID);
	void Resize(int width, int height, double sizeMult);
	void SetIterations(int iterations);
	void SetPeriodOffset(double period, double offset);
	void SetMinDeviation(double minDeviation);
	void SetDeviationCycles(int deviationCycles, bool debugDeviations);
	void SetLengthLimit(double lengthLimit);
	void SetFractal(FractalType fractal);
	void SetCustomJuliaPosition(bool use, double x, double y);
	float GetProgress();
	void Zoom(double x, double y, double amount);
	void ResetZoom();
	ComplexFloat ScreenToWorldPos(double x, double y);
	bool Draw(bool update);
protected:

	// Change these to modify the drawn fractal equations!
	static ComplexFloat FRACTAL_STARTING_FUNCTION_JULIA(ComplexFloat input, ComplexFloat extraValue)
	{ return input; };
	static ComplexFloat FRACTAL_RECURSIVE_FUNCTION_JULIA(ComplexFloat input, ComplexFloat previousValue, ComplexFloat extraValue) {
		const double JULIA_NUMBER = 0.75;
		//return previousValue * previousValue + ComplexFloat(cos(time) * JULIA_NUMBER, sin(time) * JULIA_NUMBER);
		// Julia set takes values right outside the main cartioid of the mandelbrot set
		//return previousValue * previousValue + ComplexFloat((cos(time) * 0.5 - cos(time * 2) * 0.25) * 1.01, (sin(time) * 0.5 - sin(time * 2) * 0.25)* 1.01);
		return previousValue * previousValue + extraValue;
	};
	static ComplexFloat FRACTAL_STARTING_FUNCTION_MANDEL(ComplexFloat input, ComplexFloat extraValue)
		{ return ComplexFloat(0,0); };
	static ComplexFloat FRACTAL_RECURSIVE_FUNCTION_MANDEL(ComplexFloat input, ComplexFloat previousValue, ComplexFloat extraValue) {
		return previousValue * previousValue + input;
	};

	FractalType currentFractal;
	float drawingProgress = 0;

	std::atomic_int iterations = 40;
	std::atomic<double> period = 100;
	std::atomic<double> offset = 0;
	std::atomic<double> minDeviation = 0;
	std::atomic<int> deviationCycles = 100;
	std::atomic<bool> debugDeviations = false;
	std::atomic<double> lengthLimit = 10;
	bool useCustomJuliaPosition = false;
	ComplexFloat customJuliaPosition = ComplexFloat(0, 0);

	long double totalTime = 0;
	steady_clock::time_point lastTimeAnim;
	steady_clock::time_point lastTimeDelta;
	bool disableZoom = false;
	CF_Float transformx;
	CF_Float transformy;
	CF_Float transformz;
	CF_Float lastTransformx;
	CF_Float lastTransformy;
	CF_Float lastTransformz;
	CF_Float lastLastTransformx;
	CF_Float lastLastTransformy;
	CF_Float lastLastTransformz;
	GLuint rampTexture = 0;
	GLuint fractalTexture = 0;
	std::atomic<float> *pixelBuffer, *rampColors = nullptr;
	int ramTexWidth = 0;
	int pixelBufferHeight = 0;
	int pixelBufferWidth = 0;
	float upScale = 1;
	bool fractalThreadNeedsRun = true;
	std::atomic_bool haltDrawingThread = false;
	GLFWwindow* window = nullptr;
	std::future<bool> drawFractalThread;

	std::future<bool> *drawFractalThreads;
	std::atomic<int> *threadProgress;
    std::mutex *Mutexes;
	std::future_status* drawingStatus;

	glm::vec3 clearColor = glm::vec3(0.5, 0.5, 0.5);
	static void DrawPixel(float* pixelBuffer, int pixelBufferWidth, int pixelBufferHeight, int x, int y, float r, float g, float b);
	static void DrawPixel(std::atomic<float>* pixelBuffer, int pixelBufferWidth, int pixelBufferHeight, int x, int y, float r, float g, float b);
	//static bool DrawFractal(float* pixelBuffer, int pixelBufferWidth, int pixelBufferHeight, const float* rampColors, int rampColorsWidth, glm::vec3 transform, float time, std::atomic_bool &halt);
	bool DrawFractalChunk(int index, ComplexFloat extraValue, CF_Float tfx, CF_Float tfy, CF_Float tfscale);
	void LockAllMutexes(bool haltDrawing = true);
	void UnlockAllMutexes();
};

