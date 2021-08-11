#pragma once
#include <iostream>
#include <thread>
//GLEW
#define GLEW_STATIC
#include <GL/glew.h>

//GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm/vec3.hpp>

class FractalDrawer
{
public:
	FractalDrawer(int width, int height, GLFWwindow* window);
	~FractalDrawer();
	void SetRampTexture(GLuint textureID);
	bool Draw();
protected:
	GLuint rampTexture = 0;
	float *pixelBuffer1, *pixelBuffer2, *currentPixelBuffer = nullptr;
	int pixelBufferHeight = 0;
	int pixelBufferWidth = 0;
	GLFWwindow* window = nullptr;
	void DrawPixel(float* pixelBuffer, int pixelBufferWidth, int pixelBufferHeight, int x, int y, float r, float g, float b);
	void DrawFractal(float* pixelBuffer, int pixelBufferWidth, int pixelBufferHeight, GLuint rampTexture, glm::vec3 transform);
};

