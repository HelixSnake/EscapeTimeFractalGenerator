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

//File Dialog
#include <imfilebrowser.h>

#include "FractalDrawer.h"
#include "MandelbrotFractal.h"
#include "ComplexFractal.h"

const GLint MAIN_WINDOW_WIDTH = 800;
const GLint MAIN_WINDOW_HEIGHT = 600;
const GLint UI_WINDOW_WIDTH = 500;
const GLint UI_WINDOW_HEIGHT = 480;
const int START_ITERATIONS = 100;
const double START_LENGTH_LIMIT = 100;
const double SMALL_DOUBLE_VALUE = 0.000000000000001;
const double ZOOM_PER_SECOND = 2.0;
const char* COLOR_RAMP_FILENAME = "colorRamp.png";
const char* IMGUI_GLSL_VERSION = "#version 130";

struct FractalInfo
{
	//need defaults: if these values are too high it will cause the program to hang
	int iterations = 40;
	double upscale = 1;
	double period = 100;
	double offset = 0;
	double minDeviation = 0;
	int deviationCycles = 1000;
	bool debugDeviations = false;
	double lengthLimit = 10;
	bool animate = false;
	bool useCustomJulPos = false;
	bool showAdvancedOptions = false;
	double CustomJulPosX = 0;
	double CustomJulPosY = 0;
	FractalType type = FractalType::Julia;
};

void ClearPixelBuffer(float* buffer, int size)
{
	for (int i = 0; i < size; i++)
	{
		buffer[i] = 0.0;
	}
}


GLuint LoadRampTexture(std::string filename)
{
	GLuint textureID = 0;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	int width, height, channels;
	unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, &channels, SOIL_LOAD_RGB);
	if (image == nullptr)
	{
		std::cout << "Ramp texture not found" << std::endl;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	return textureID;
}

GLuint LoadRampTexture()
{
	return LoadRampTexture(COLOR_RAMP_FILENAME);
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

void RenderUIWindow(GLFWwindow* uiWindow, FractalDrawer* fractalDrawer, bool& uiUpdate, bool& regenBuffer, FractalInfo& fractalInfo, ImGui::FileBrowser &rampTexFileBrowser)
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
	bool mainWindowOpen;
	ImGui::Begin("Main Window", &mainWindowOpen, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
	ImGui::SetWindowSize(ImVec2(UI_WINDOW_WIDTH, UI_WINDOW_HEIGHT));
	ImGui::SetWindowPos(ImVec2(0, 0));
	ImGui::Text("Left click to zoom in, right click to zoom out, \nmiddle click or J to choose custom Julia set value");
	ImGui::InputInt("Iterations:", &fractalInfo.iterations, 10);
	fractalInfo.iterations = min(fractalInfo.iterations, 10000000); //Too many iterations will cause the program to hang, since threads are halted on a per pixel basis
	ImGui::Text("Upsample can be any value between 0 and 1, or 2.0, or 4.0 \nRendering speed scales roughly with the upsample value squared");

	ImGui::InputDouble("Upsample", &fractalInfo.upscale);
	fractalInfo.upscale = glm::clamp(fractalInfo.upscale, 0.0, 4.0);
	//keep upscale a power of 2 if it's more than 1
	if (fractalInfo.upscale > 1.0f)
	{
		fractalInfo.upscale = pow(2, floor(log2(fractalInfo.upscale)));
	}
	ImGui::InputDouble("Period", &fractalInfo.period);
	//this is annoying but imgui makes you do this :(
	float tempOffset = fractalInfo.offset;
	if (ImGui::SliderFloat("Offset", &tempOffset, 0.0, 1.0))
	{
		uiUpdate = true;
	}
	fractalInfo.offset = tempOffset;
	ImGui::Text("Fractal Type:");
	bool fractalIsJulia = fractalInfo.type == FractalType::Julia;
	bool fractalIsMandelbrot = fractalInfo.type == FractalType::Mandelbrot;
	if (ImGui::Checkbox("Julia", &fractalIsJulia))
	{
		fractalInfo.type = FractalType::Julia;
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("Mandelbrot", &fractalIsMandelbrot))
	{
		fractalInfo.type = FractalType::Mandelbrot;
	}
	if (ImGui::Checkbox("Animate!", &fractalInfo.animate))
	{
		if (fractalInfo.animate)
		{
			fractalInfo.useCustomJulPos = false;
		}
	};
	if (ImGui::Checkbox("Custom Julia Position (middle mouse click or J)", &fractalInfo.useCustomJulPos))
	{
		if (fractalInfo.useCustomJulPos)
		{
			fractalInfo.animate = false;
		}
		uiUpdate = true;
	}
	if (fractalInfo.useCustomJulPos)
	{
		ImGui::InputDouble("Custom Position X", &fractalInfo.CustomJulPosX, 0.0, 0.0, " % .16f");
		ImGui::InputDouble("Custom Position Y", &fractalInfo.CustomJulPosY, 0.0, 0.0, " % .16f");
	}
	if (ImGui::Button("Update"))
	{
		fractalDrawer->SetIterations(fractalInfo.iterations);
		fractalDrawer->SetPeriodOffset(fractalInfo.period, fractalInfo.offset);
		fractalDrawer->SetMinDeviation(fractalInfo.minDeviation);
		fractalDrawer->SetDeviationCycles(fractalInfo.deviationCycles, fractalInfo.debugDeviations);
		fractalDrawer->SetLengthLimit(fractalInfo.lengthLimit);
		fractalDrawer->SetFractal(fractalInfo.type);
		uiUpdate = true;
		// We need to regenerate the pixel buffer due to changed texture dimensions in case upscale has changed
		regenBuffer = true;
	}
	if (ImGui::Button("Reset Zoom"))
	{
		fractalDrawer->ResetZoom();
		uiUpdate = true;
	}
	if (ImGui::Button("Choose Ramp Texture"))
	{
		rampTexFileBrowser.Open();
	}
	ImGui::SameLine();
	if (ImGui::Button("Default"))
	{
		GLuint rampTexture = LoadRampTexture();
		fractalDrawer->SetRampTexture(rampTexture);
		glDeleteTextures(1, &rampTexture);
		uiUpdate = true;
	}
	ImGui::Checkbox("Show advanced options", &fractalInfo.showAdvancedOptions);
	if (fractalInfo.showAdvancedOptions)
	{
		ImGui::InputDouble("Length Limit", &fractalInfo.lengthLimit, 0.0, 0.0, "%.3f");
		ImGui::Text("Set this value to something small to improve rendering time");
		ImGui::InputDouble("Minimum Deviation", &fractalInfo.minDeviation, SMALL_DOUBLE_VALUE, 0.0, "%.15f");
		ImGui::InputInt("Deviation Cycles", &fractalInfo.deviationCycles);
		ImGui::Checkbox("Show Deviation Iterations", &fractalInfo.debugDeviations);
	}

	ImGui::Checkbox("Live Rendering", &fractalDrawer->liveUpdate);
	ImGui::ProgressBar(fractalDrawer->GetProgress());
	ImGui::End();
	rampTexFileBrowser.SetWindowSize(UI_WINDOW_WIDTH, UI_WINDOW_HEIGHT);
	ImGui::SetWindowPos(ImVec2(0, 0));
	rampTexFileBrowser.Display();
	if (rampTexFileBrowser.HasSelected())
	{
		GLuint rampTexture = LoadRampTexture(rampTexFileBrowser.GetSelected().string());
		fractalDrawer->SetRampTexture(rampTexture);
		glDeleteTextures(1, &rampTexture);
		uiUpdate = true;
		rampTexFileBrowser.ClearSelected();
	}

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
	glfwWindowHint(GLFW_FLOATING, GL_TRUE);
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
	glDeleteTextures(1, &rampTexture);
	//initial delta time start
	auto deltaTimeStart = std::chrono::high_resolution_clock::now();
	bool animateFractal = true;
	bool SpaceBarPressed = false;
	bool juliaPosUpdate = false;
	bool uiUpdate = false;
	bool regenBuffer = false;
	//fractal properties
	FractalInfo fracInfo;
	fracInfo.iterations = START_ITERATIONS;
	fracInfo.upscale = 1;
	fracInfo.period = 1; 
	fracInfo.offset = 0;
	fracInfo.minDeviation = SMALL_DOUBLE_VALUE;
	fracInfo.lengthLimit = START_LENGTH_LIMIT;
	fracInfo.animate = false;
	fracInfo.useCustomJulPos = false;
	fracInfo.CustomJulPosX = 0;
	fracInfo.CustomJulPosY = 0;
	fractalDrawer->SetIterations(fracInfo.iterations);
	fractalDrawer->SetPeriodOffset(fracInfo.period, fracInfo.offset);
	fractalDrawer->SetMinDeviation(fracInfo.minDeviation);
	fractalDrawer->SetLengthLimit(fracInfo.lengthLimit);
	fractalDrawer->SetFractal(fracInfo.type);
	fractalDrawer->SetCustomJuliaPosition(fracInfo.useCustomJulPos, fracInfo.CustomJulPosX, fracInfo.CustomJulPosY);

	//file dialog
	ImGui::FileBrowser rampTexFileDialog;
	rampTexFileDialog.SetTitle("Custom Ramp Texture");
	rampTexFileDialog.SetTypeFilters({ ".png" });

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
		int middleMBstate = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);
		int jKeyState = glfwGetKey(window, GLFW_KEY_J);
		int jKeyState2 = glfwGetKey(uiWindow, GLFW_KEY_J);
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

		bool chooseJuliaValue = middleMBstate == GLFW_PRESS || jKeyState == GLFW_PRESS || jKeyState2 == GLFW_PRESS;
		if (chooseJuliaValue)
		{
			ComplexFloat newPos = fractalDrawer->ScreenToWorldPos(mbxpos, 1 - mbypos);
			fracInfo.CustomJulPosX = newPos.real;
			fracInfo.CustomJulPosY = newPos.imaginary;
			fracInfo.useCustomJulPos = true;
			fracInfo.animate = false;
			fractalDrawer->SetCustomJuliaPosition(true, fracInfo.CustomJulPosX, fracInfo.CustomJulPosY);
		}
		juliaPosUpdate = chooseJuliaValue && fracInfo.type == FractalType::Julia;

		// Render 
		// Clear the colorbuffer 
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Refresh pixelBuffer on resize (TODO)
		int nextWindowWidth = 0;
		int nextWindowHeight = 0;
		glfwGetWindowSize(window, &nextWindowWidth, &nextWindowHeight);
		bool updateOnResize = false;
		// Regenerate the fractal drawer's pixel buffer due to changed texture dimensions
		if (currentWindowWidth != nextWindowWidth || currentWindowHeight != nextWindowHeight || regenBuffer)
		{
			currentWindowWidth = nextWindowWidth;
			currentWindowHeight = nextWindowHeight;
			updateOnResize = true;
			fractalDrawer->Resize(currentWindowWidth, currentWindowHeight, fracInfo.upscale);
		}
		fractalDrawer->SetCustomJuliaPosition(fracInfo.useCustomJulPos, fracInfo.CustomJulPosX, fracInfo.CustomJulPosY);
		if (uiUpdate)
		{
			fractalDrawer->SetPeriodOffset(fracInfo.period, fracInfo.offset);
		}
		// Draw fractal
		fractalDrawer->Draw(updateOnResize || fracInfo.animate || juliaPosUpdate || uiUpdate);

		glfwSwapBuffers(window);

		uiUpdate = false;
		regenBuffer = false;
		//Render IMGUI stuff
		RenderUIWindow(uiWindow, fractalDrawer, uiUpdate, regenBuffer, fracInfo, rampTexFileDialog);
		fractalDrawer->enableAnimation = fracInfo.animate;
	}
	delete fractalDrawer;
	glfwTerminate();

	return EXIT_SUCCESS;
}
