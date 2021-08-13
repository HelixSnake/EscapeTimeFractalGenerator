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
	auto deltaTimeStart = std::chrono::high_resolution_clock::now();
	while (!glfwWindowShouldClose(window))
	{
		auto deltaTimeEnd = std::chrono::high_resolution_clock::now();
		float deltaTime = std::chrono::duration_cast<std::chrono::nanoseconds>(deltaTimeEnd - deltaTimeStart).count() / 1000000000.0;
		deltaTimeStart = std::chrono::high_resolution_clock::now();
		// Check if any events have been activiated (key pressed, 
		//mouse moved etc.) and call corresponding response functions 
		glfwPollEvents();

		//zoom?
		int leftMBstate = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
		int rightMBstate = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
		double mbxpos, mbypos;
		glfwGetCursorPos(window, &mbxpos, &mbypos);
		mbxpos /= currentWindowWidth;
		mbypos /= currentWindowHeight;
		if (leftMBstate == GLFW_PRESS)
		{
			fractalDrawer->Zoom(mbxpos, 1 - mbypos, pow(0.5, deltaTime));
		}
		else if (rightMBstate == GLFW_PRESS)
		{
			fractalDrawer->Zoom(mbxpos, 1 - mbypos, pow(2, deltaTime));
		}

		// Render 
		// Clear the colorbuffer 
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Refresh pixelBuffer on resize (TODO)
		int nextWindowWidth = 0;
		int nextWindowHeight = 0;
		glfwGetWindowSize(window, &nextWindowWidth, &nextWindowHeight);
		//TODO: make threadsafe
		bool shouldUpdate = false;
		if (currentWindowWidth != nextWindowWidth || currentWindowHeight != nextWindowHeight)
		{
			currentWindowWidth = nextWindowWidth;
			currentWindowHeight = nextWindowHeight;
			shouldUpdate = true;
			fractalDrawer->Resize(currentWindowWidth, currentWindowHeight);
		}
		// Draw OpenGL 
		fractalDrawer->Draw(shouldUpdate);

		glfwSwapBuffers(window);
	}
	glfwTerminate();

	return EXIT_SUCCESS;
}
