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
#define IMGUI_IMPL_OPENGL_LOADER_GLEW
#include <imgui.h>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

//GLM
#include <glm/vec3.hpp>
#include <glm/common.hpp>

#include "FractalDrawer.h"
#include "MandelbrotFractal.h"
#include "ComplexFractal.h"

const GLint MAIN_WINDOW_WIDTH = 800;
const GLint MAIN_WINDOW_HEIGHT = 600;
const GLint UI_WINDOW_WIDTH = 500;
const GLint UI_WINDOW_HEIGHT =300;
const double SMALL_DOUBLE_VALUE = 0.000000000000001;
const double ZOOM_PER_SECOND = 2.0;
const char* COLOR_RAMP_FILENAME = "colorRamp.png";
const char* IMGUI_GLSL_VERSION = "#version 130";

struct FractalInfo
{
	//need defaults: if these values are too high it will cause the program to hang
	int iterations = 40;
	float upscale = 1;
	float period = 100;
	double minDeviation = 0;
};

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

void ImgUIInitialize(GLFWwindow* uiWindow, const char* glsl_version, ImGuiIO& io)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = ImGui::GetIO();
	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(uiWindow, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
}

void RenderUIWindow(GLFWwindow* uiWindow, bool& updateButton, FractalInfo& fractalInfo)
{
	glfwMakeContextCurrent(uiWindow);
	glfwPollEvents();
	glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
	glClear(GL_COLOR_BUFFER_BIT);

	// feed inputs to dear imgui, start new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// render your GUI
	ImGui::Begin("Main Window");
	ImGui::SetWindowSize(ImVec2(0, 0));
	ImGui::InputInt("Iterations:", &fractalInfo.iterations);
	ImGui::InputFloat("Upscale", &fractalInfo.upscale);
	ImGui::InputFloat("Period", &fractalInfo.period);
	fractalInfo.upscale = glm::clamp(fractalInfo.upscale, 0.0f, 8.0f);
	ImGui::Text("Set this value to something small to improve rendering time");
	ImGui::InputDouble("Minimum Deviation", &fractalInfo.minDeviation, SMALL_DOUBLE_VALUE, 0.0, "%.15f");
	if (ImGui::Button("Update"))
	{
		updateButton = true;
	}
	ImGui::End();

	// Render dear imgui into screen
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	int display_w, display_h;
	glfwGetFramebufferSize(uiWindow, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	glfwSwapBuffers(uiWindow);
}

int main(int argc, char* argv[])
{
	glfwInit();
	// Create main window and UI window
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	GLFWwindow* window = glfwCreateWindow(MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT, "Escape Time Fractal Generator", nullptr, nullptr);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	GLFWwindow* uiWindow = glfwCreateWindow(UI_WINDOW_WIDTH, UI_WINDOW_HEIGHT, "Escape Time Fractal Generator UI", nullptr, nullptr);
	int screenWidth, screenHeight;
	glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
	if (window == nullptr || uiWindow == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}
	glfwMakeContextCurrent(window);

	//initialize glew
	glewExperimental = GL_TRUE;
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}
	//initialize imgUI
	ImGuiIO io;
	ImgUIInitialize(uiWindow, IMGUI_GLSL_VERSION, io);
	// Define the viewport dimensions 
	glViewport(0, 0, screenWidth, screenHeight);
	int currentWindowWidth = 0;
	int currentWindowHeight = 0;
	glfwGetWindowSize(window, &currentWindowWidth, &currentWindowHeight);
	// initialize fractal drawer class
	FractalDrawer* fractalDrawer = new FractalDrawer(currentWindowWidth, currentWindowHeight, window);
	//load and set ramp texture
	GLuint rampTexture = LoadRampTexture();
	fractalDrawer->SetRampTexture(rampTexture);
	//initial delta time start
	auto deltaTimeStart = std::chrono::high_resolution_clock::now();
	bool animateFractal = true;
	bool SpaceBarPressed = false;
	bool updateButton = false;
	//fractal properties
	FractalInfo fracInfo;
	fracInfo.iterations = 40;
	fracInfo.upscale = 1;
	fracInfo.period = 1; 
	fracInfo.minDeviation = SMALL_DOUBLE_VALUE;
	fractalDrawer->SetIterations(fracInfo.iterations);
	fractalDrawer->SetPeriod(fracInfo.period);
	fractalDrawer->SetMinDeviation(fracInfo.minDeviation);
	while (!glfwWindowShouldClose(window) && !glfwWindowShouldClose(uiWindow))
	{
		glfwMakeContextCurrent(window);
		// calculate delta time
		auto deltaTimeEnd = std::chrono::high_resolution_clock::now();
		float deltaTime = std::chrono::duration_cast<std::chrono::nanoseconds>(deltaTimeEnd - deltaTimeStart).count() / 1000000000.0;
		deltaTimeStart = std::chrono::high_resolution_clock::now();
		// Check if any events have been activiated (key pressed, 
		//mouse moved etc.) and call corresponding response functions 
		glfwPollEvents();

		//zoom?
		int leftMBstate = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
		int rightMBstate = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
		int spaceBar = glfwGetKey(window, GLFW_KEY_SPACE);
		double mbxpos, mbypos;
		glfwGetCursorPos(window, &mbxpos, &mbypos);
		mbxpos /= currentWindowWidth;
		mbypos /= currentWindowHeight;
		if (leftMBstate == GLFW_PRESS)
		{
			fractalDrawer->Zoom(mbxpos, 1 - mbypos, pow(1.0 / ZOOM_PER_SECOND, deltaTime));
		}
		else if (rightMBstate == GLFW_PRESS)
		{
			fractalDrawer->Zoom(mbxpos, 1 - mbypos, pow(ZOOM_PER_SECOND, deltaTime));
		}
		if (spaceBar == GLFW_PRESS)
		{
			if (!SpaceBarPressed)
			animateFractal = !animateFractal;
			SpaceBarPressed = true;
		}
		else
		{
			SpaceBarPressed = false;
		}
		fractalDrawer->enableAnimation = animateFractal;

		// Render 
		// Clear the colorbuffer 
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Refresh pixelBuffer on resize (TODO)
		int nextWindowWidth = 0;
		int nextWindowHeight = 0;
		glfwGetWindowSize(window, &nextWindowWidth, &nextWindowHeight);
		//TODO: make threadsafe
		bool updateOnResize = false;
		if (currentWindowWidth != nextWindowWidth || currentWindowHeight != nextWindowHeight || updateButton)
		{
			currentWindowWidth = nextWindowWidth;
			currentWindowHeight = nextWindowHeight;
			updateOnResize = true;
			fractalDrawer->Resize(currentWindowWidth, currentWindowHeight, fracInfo.upscale);
		}
		// Draw OpenGL 
		//fractalDrawer->Draw(shouldUpdate || animateFractal);
		if (updateButton)
		{
			fractalDrawer->SetIterations(fracInfo.iterations);
			fractalDrawer->SetPeriod(fracInfo.period);
			fractalDrawer->SetMinDeviation(fracInfo.minDeviation);
		}
		fractalDrawer->Draw(updateOnResize || updateButton);

		glfwSwapBuffers(window);

		updateButton = false;
		//Render IMGUI stuff
		RenderUIWindow(uiWindow, updateButton, fracInfo);
	}
	delete fractalDrawer;
	glfwTerminate();

	return EXIT_SUCCESS;
}
