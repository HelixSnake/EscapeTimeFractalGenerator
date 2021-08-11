#include <iostream>
#include <thread>
//GLEW
#define GLEW_STATIC
#include <GL/glew.h>

//SOIL
#include <SOIL.h>
//GLFW
#include <GLFW/glfw3.h>

//IMGUI
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

//GLM
#include <glm/vec3.hpp>

#include "FractalDrawer.h"
#include "MandelbrotFractal.h"
#include "ComplexFractal.h"

const GLint MAIN_WINDOW_WIDTH = 800;
const GLint MAIN_WINDOW_HEIGHT = 600;
const char* COLOR_RAMP_FILENAME = "colorRamp.png";

void ClearPixelBuffer(float* buffer, int size)
{
	for (int i = 0; i < size; i++)
	{
		buffer[i] = 0.0;
	}
}

GLuint LoadRampTexture()
{
	GLuint textureID = 0;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	int width, height, channels;
	unsigned char* image = SOIL_load_image(COLOR_RAMP_FILENAME, &width, &height, &channels, SOIL_LOAD_RGB);
	if (image == nullptr)
	{
		std::cout << "Ramp texture not found" << std::endl;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	//SOIL_free_image_data(image);
	return textureID;
}

void DrawPixel(float* pixelBuffer, int pixelBufferWidth, int pixelBufferHeight, int x, int y, float r, float g, float b)
{
	if (x < 0 || x >= pixelBufferWidth || y < 0 || y >= pixelBufferHeight) return;
	int startIndex = 3 * (y * pixelBufferWidth + x);
	pixelBuffer[startIndex] = r;
	pixelBuffer[startIndex + 1] = g;
	pixelBuffer[startIndex + 2] = b;
}

void DrawFractal(float* pixelBuffer, int pixelBufferWidth, int pixelBufferHeight, GLuint rampTexture, glm::vec3 transform)
{
	ComplexFractal fractal = ComplexFractal(40);
	fractal.SetStartingFunction([](ComplexFloat input) {return input; });
	fractal.SetFunction([](ComplexFloat input, ComplexFloat previousValue) {return previousValue * previousValue + ComplexFloat(input.imaginary, input.real); });
	glBindTexture(GL_TEXTURE_2D, rampTexture);
	GLint ramTexWidth;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &ramTexWidth);
	GLfloat* rampColor = new GLfloat[ramTexWidth*3];
	glGetnTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, ramTexWidth*3*sizeof(GLfloat), rampColor);
	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		std::cout << "OpenGL error: " << err << std::endl;
	}
	for (int i = 0; i < pixelBufferHeight; i++)
	{
		for (int j = 0; j < pixelBufferWidth; j++)
		{
			float x = (float)j / pixelBufferWidth;
			float y = (float)i / pixelBufferHeight;
			x = (x - transform.x) * transform.z * pixelBufferWidth / pixelBufferHeight;
			y = (y - transform.y) * transform.z;
			float value = fractal.CalculateEscapeTime(x, y);
			//float value = 0;
			int rampIndex = ((int)((float)ramTexWidth * (1.0 - pow(value, 0.4))));
			if (rampIndex > ramTexWidth - 1) rampIndex = ramTexWidth - 1;
			rampIndex *= 3;
			if (value == 0)
			{
				DrawPixel(pixelBuffer, pixelBufferWidth, pixelBufferHeight, j, i, 0, 0, 0);
			}
			else
			{
				DrawPixel(pixelBuffer, pixelBufferWidth, pixelBufferHeight, j, i, rampColor[rampIndex], rampColor[rampIndex + 1], rampColor[rampIndex + 2]);
				//DrawPixel(pixelBuffer, pixelBufferWidth, pixelBufferHeight, j, i, value, value, value);
			}
		}
	}
	delete[] rampColor;
}
bool Draw(GLFWwindow* window, float* pixelBuffer, int pixelBufferWidth, int pixelBufferHeight, GLuint rampTexture)
{
	//Draw Fractal
	DrawFractal(pixelBuffer, pixelBufferWidth, pixelBufferHeight, rampTexture, glm::vec3(0.5, 0.5, 2));

	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pixelBufferWidth, pixelBufferHeight, 0, GL_RGB, GL_FLOAT, pixelBuffer);

	GLuint fbo = 0;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, tex, 0);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); 
	int width = 0;
	int height = 0;
	glfwGetWindowSize(window, &width, &height);
	glBlitFramebuffer(0, 0, pixelBufferWidth, pixelBufferHeight, 0, 0, width, height,
		GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glDeleteFramebuffers(1, &fbo);
	glDeleteTextures(1, &tex);
	return true;
}

int main(int argc, char* argv[])
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	GLFWwindow* window = glfwCreateWindow(MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT, "Escape Time Fractal Generator", nullptr, nullptr);
	int screenWidth, screenHeight;
	glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}
	// Define the viewport dimensions 
	glViewport(0, 0, screenWidth, screenHeight);
	int currentWindowWidth = 0;
	int currentWindowHeight = 0;
	glfwGetWindowSize(window, &currentWindowWidth, &currentWindowHeight);
	//int pixelBufferSize = 3 * currentWindowWidth * currentWindowHeight;
	//float* pixelBuffer1 = new float[pixelBufferSize];
	//float* pixelBuffer2 = new float[pixelBufferSize];
	//ClearPixelBuffer(pixelBuffer1, pixelBufferSize);
	//ClearPixelBuffer(pixelBuffer2, pixelBufferSize);
	FractalDrawer* fractalDrawer = new FractalDrawer(currentWindowWidth, currentWindowHeight, window);
	GLuint rampTexture = LoadRampTexture();
	fractalDrawer->SetRampTexture(rampTexture);
	while (!glfwWindowShouldClose(window))
	{
		// Check if any events have been activiated (key pressed, 
		//mouse moved etc.) and call corresponding response functions 
		glfwPollEvents();

		// Render 
		// Clear the colorbuffer 
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Refresh pixelBuffer on resize (TODO)
		int nextWindowWidth = 0;
		int nextWindowHeight = 0;
		glfwGetWindowSize(window, &nextWindowWidth, &nextWindowHeight);
		//TODO: make threadsafe
		/*
		if (currentWindowWidth != nextWindowWidth || currentWindowHeight != nextWindowHeight)
		{
			delete[] pixelBuffer1;
			delete[] pixelBuffer2;
			currentWindowWidth = nextWindowWidth;
			currentWindowHeight = nextWindowHeight;
			pixelBuffer1 = new float[3 * currentWindowWidth * currentWindowHeight];
			pixelBuffer2 = new float[3 * currentWindowWidth * currentWindowHeight];
		}
		*/

		// Draw OpenGL 
		fractalDrawer->Draw();

		glfwSwapBuffers(window);
	}
	glfwTerminate();

	return EXIT_SUCCESS;
}
