#include <iostream>
//GLEW
#define GLEW_STATIC
#include <GL/glew.h>

//GLFW
#include <GLFW/glfw3.h>

//IMGUI
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "JuliaFractal.h"


const GLint MAIN_WINDOW_WIDTH = 800;
const GLint MAIN_WINDOW_HEIGHT = 600;

bool Draw(GLFWwindow* window)
{
	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// Black/white checkerboard
	float pixels[] = {
		0.0f, 0.0f, 0.0f,   1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,   0.0f, 0.0f, 0.0f
	};
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_FLOAT, pixels);

	GLuint fbo = 0;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, tex, 0);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); 
	int width = 0;
	int height = 0;
	glfwGetWindowSize(window, &width, &height);
	glBlitFramebuffer(0, 0, 2, 2, 0, 0, width, height,
		GL_COLOR_BUFFER_BIT, GL_NEAREST);

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
	while (!glfwWindowShouldClose(window))
	{
		// Check if any events have been activiated (key pressed, 
		//mouse moved etc.) and call corresponding response functions 
		glfwPollEvents();

		// Render 
		// Clear the colorbuffer 
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Draw OpenGL 
		Draw(window);

		glfwSwapBuffers(window);
	}
	glfwTerminate();

	return EXIT_SUCCESS;
}
