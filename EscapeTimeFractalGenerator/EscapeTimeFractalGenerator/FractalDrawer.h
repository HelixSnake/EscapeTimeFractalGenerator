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

using namespace std::chrono;

const int NUM_FRACTAL_DRAW_THREADS = 16;

class FractalDrawer
{
public:
	FractalDrawer(int width, int height, GLFWwindow* window);
	~FractalDrawer();
	void SetRampTexture(GLuint textureID);
	void Resize(int width, int height);
	void Zoom(float x, float y, float amount);
	bool Draw(bool update);
protected:
	steady_clock::time_point startTime;
	glm::vec3 transform;
	glm::vec3 lastTransform;
	glm::vec3 lastlastTransform;
	GLuint rampTexture = 0;
	GLuint fractalTexture = 0;
	float *pixelBuffer, *rampColors = nullptr;
	int ramTexWidth = 0;
	int pixelBufferHeight = 0;
	int pixelBufferWidth = 0;
	bool fractalThreadNeedsRun = true;
	std::atomic_bool haltDrawingThread = false;
	GLFWwindow* window = nullptr;
	std::future<bool> drawFractalThread;
	std::future<bool> drawFractalThreads[NUM_FRACTAL_DRAW_THREADS];
	std::atomic<int> threadProgress[NUM_FRACTAL_DRAW_THREADS];
    std::mutex Mutexes[NUM_FRACTAL_DRAW_THREADS];
	static void DrawPixel(float* pixelBuffer, int pixelBufferWidth, int pixelBufferHeight, int x, int y, float r, float g, float b);
	static bool DrawFractal(float* pixelBuffer, int pixelBufferWidth, int pixelBufferHeight, const float* rampColors, int rampColorsWidth, glm::vec3 transform, float time, std::atomic_bool &halt);
	bool DrawFractalChunk(int index, float time, glm::vec3 transform);
	void LockAllMutexes();
	void UnlockAllMutexes();
};

