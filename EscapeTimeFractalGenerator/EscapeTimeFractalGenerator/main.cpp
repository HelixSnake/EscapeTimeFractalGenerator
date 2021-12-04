#include <iostream>
#include <thread>

//SOIL
#include <SOIL.h>

//GLEW
#define GLEW_STATIC
#include <GL/glew.h>

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
#include "FractalInterpreter.h"
#include "MandelbrotFractal.h"
#include "ComplexFractal.h"
#include "QuadDrawer.h"
#include "FractalSmoothZoomer.h"

const GLint MAIN_WINDOW_WIDTH = 800;
const GLint MAIN_WINDOW_HEIGHT = 600;
const GLint UI_WINDOW_WIDTH = 500;
const GLint UI_WINDOW_HEIGHT = 600;
const int START_ITERATIONS = 100;
const double START_LENGTH_LIMIT = 100;
const double SMALL_DOUBLE_VALUE = 0.000000000000001;
const double ZOOM_PER_SECOND = 2.0;
const char* COLOR_RAMP_FILENAME = "colorRamp.png";
const char* IMGUI_GLSL_VERSION = "#version 130";

const ZoomTransform DEFAULT_ZOOMTRANSFORM = ZoomTransform(0.5, 0.5, 3);

using namespace std::chrono;

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
	bool liveUpdate = false;
	double CustomJulPosX = 0;
	double CustomJulPosY = 0;
	double DistortionVectorX = 0;
	double DistortionVectorY = 0;
	int power = 2;
	FractalDictionary::FractalType type = FractalDictionary::FractalType::Julia;
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

void DisplayFractalTypeCheckbox(FractalDictionary::FractalType type, FractalInfo &fractalInfo)
{
	bool isType = type == fractalInfo.type;
	FractalDictionary::FractalTypeInfo typeInfo = FractalDictionary::GetInfo(type);
	if (ImGui::Checkbox(typeInfo.name, &isType))
	{
		fractalInfo.type = type;
	}
}

void DisplayToolTip(const char* s)
{
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip(s);
	}
}

//TODO: Long argument list is sign of code smell - find way to move this to its own class
void RenderUIWindow(GLFWwindow* uiWindow, FractalDrawer* fractalDrawer, bool& updateDrawer, bool& updateInterpreter, bool& regenBuffer, FractalInfo& fractalInfo, FractalInterpreter& fractalInterpreter, FractalSmoothZoomer &smoothZoomer, ZoomTransform &zoomTransform, ImGui::FileBrowser &rampTexFileBrowser)
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
	ImGui::InputInt("Iterations", &fractalInfo.iterations, 10);
	DisplayToolTip("Higher number = better detail but slower performance");
	ImGui::InputDouble("Length Limit", &fractalInfo.lengthLimit, 0.0, 0.0, "%.3f");
	DisplayToolTip("Higher number = more precise / accurate results.\nLower values perform slightly better.");
	fractalInfo.iterations = min(fractalInfo.iterations, 10000000); //Too many iterations will cause the program to hang, since threads are halted on a per pixel basis

	ImGui::InputDouble("Upsample", &fractalInfo.upscale);
	DisplayToolTip("Upsample can be any value between 0 and 1, or 2.0, or 4.0 \nRendering speed scales roughly with the upsample value squared");
	fractalInfo.upscale = glm::clamp(fractalInfo.upscale, 0.0, 4.0);
	//keep upscale a power of 2 if it's more than 1
	if (fractalInfo.upscale > 1.0f)
	{
		fractalInfo.upscale = pow(2, floor(log2(fractalInfo.upscale)));
	}
	if (ImGui::InputDouble("Period", &fractalInfo.period))
	{
		updateInterpreter = true;
	}
	DisplayToolTip("Changes how often the ramp texture repeats");
	//this is annoying but imgui makes you do this :(
	float tempOffset = fractalInfo.offset;
	if (ImGui::SliderFloat("Offset", &tempOffset, 0.0, 1.0))
	{
		updateInterpreter = true;
	}
	DisplayToolTip("Changes the offset to the ramp texture");
	fractalInfo.offset = tempOffset;
	const char* juliaTooltip = "Press J when mousing over a certain area of the Mandelbrot version\n to get a Julia set with similar properties";
	ImGui::Text("Fractal Type (mouse over for tips):");
	DisplayFractalTypeCheckbox(FractalDictionary::FractalType::Mandelbrot, fractalInfo);
	DisplayToolTip("Formula: n = ((n-1)^power + c);");
	ImGui::SameLine();
	DisplayFractalTypeCheckbox(FractalDictionary::FractalType::Julia, fractalInfo);
	DisplayToolTip(juliaTooltip);
	DisplayFractalTypeCheckbox(FractalDictionary::FractalType::BurningShip, fractalInfo);
	DisplayToolTip("Formula: n = ((n-1)^power + c); n.x = abs(n.x); n.y = abs(n.y);");
	ImGui::SameLine();
	DisplayFractalTypeCheckbox(FractalDictionary::FractalType::BurningJulia, fractalInfo);
	DisplayToolTip(juliaTooltip);
	DisplayFractalTypeCheckbox(FractalDictionary::FractalType::ReflectedMandelbrot, fractalInfo);
	DisplayToolTip("Formula: n = ((n - 1) ^ power + c); n -= v * dot(n,v) * 2;\nHold V to set the reflection vector");
	ImGui::SameLine();
	DisplayFractalTypeCheckbox(FractalDictionary::FractalType::ReflectedJulia, fractalInfo);
	DisplayToolTip(juliaTooltip);
	DisplayFractalTypeCheckbox(FractalDictionary::FractalType::SpunMandelbrot, fractalInfo);
	DisplayToolTip("Formula: n = ((n - 1) ^ power + c); n += n / |n| * v;\nHold V to set the spin vector");
	ImGui::SameLine();
	DisplayFractalTypeCheckbox(FractalDictionary::FractalType::SpunJulia, fractalInfo);
	DisplayToolTip(juliaTooltip);
	DisplayFractalTypeCheckbox(FractalDictionary::FractalType::Peacock, fractalInfo);
	DisplayToolTip("Formula: n = ((v.x ^ (n - 1)) ^ power + c);\nv is Additional Constant\nSet the X value of Additional Constant to get this fractal to work\nGood values are 0.5 or 2\nSet Length Limit to a higher value for more correct results");
	ImGui::SameLine();
	DisplayFractalTypeCheckbox(FractalDictionary::FractalType::PeacockJulia, fractalInfo);
	DisplayToolTip("Set the X value of Additional Constant to get this fractal to work\nGood values are 0.5 or 2\nSet Length Limit to a higher value for more correct results");
	DisplayFractalTypeCheckbox(FractalDictionary::FractalType::CosineMandelbrot, fractalInfo);
	DisplayToolTip("Formula: n = (cos(n - 1)) ^ power + c);\nSet Length Limit to a higher value for more correct results");
	ImGui::SameLine();
	DisplayFractalTypeCheckbox(FractalDictionary::FractalType::CosineJulia, fractalInfo);
	DisplayToolTip("Set Length Limit to a higher value for more correct results");
	DisplayFractalTypeCheckbox(FractalDictionary::FractalType::ReciprocalMandelbrot, fractalInfo);
	DisplayToolTip("Formula: n = (1/((n-1)^power) + c);\nTechnically only diverges at very specific points\nSet Length Limit to a higher value for more correct results");
	ImGui::SameLine();
	DisplayFractalTypeCheckbox(FractalDictionary::FractalType::ReciprocalJulia, fractalInfo);
	DisplayToolTip("Technically only diverges at very specific Julia Positions\nTo find these positions, set Length Limit to 100.0\nchoose a point on the Mandelbrot version,\n then zoom into one of the circles\nIncrease the Length Limit and zoom in as the circle becomes smaller\nMouse over the small circle and press J\nReset your zoom and enable the Julia version");
	DisplayFractalTypeCheckbox(FractalDictionary::FractalType::SuperMandelbrot, fractalInfo);
	DisplayToolTip("Formula: n = ((n-1)^v + c);\nHold V to set the complex exponent");
	ImGui::SameLine();
	DisplayFractalTypeCheckbox(FractalDictionary::FractalType::SuperJulia, fractalInfo);
	DisplayToolTip("Hold V to set the complex exponent, hold J to set Julia Position");

	ImGui::InputInt("Fractal Power", &fractalInfo.power);
	DisplayToolTip("The power used in the fractal equation, when applicable.\n Limited to between 1 and 7");
	fractalInfo.power = glm::clamp(fractalInfo.power, 1, 7); //to support values over 7 more values need to be fine tuned in ComplexFractal.cpp
	if (ImGui::Checkbox("Animate!", &fractalInfo.animate))
	{
		if (fractalInfo.animate)
		{
			fractalInfo.useCustomJulPos = false;
		}
	};
	DisplayToolTip("Animate the Julia Position along a preset path");
	if (ImGui::Checkbox("Custom Julia Position (middle mouse click or J)", &fractalInfo.useCustomJulPos))
	{
		if (fractalInfo.useCustomJulPos)
		{
			fractalInfo.animate = false;
			fractalInfo.useCustomJulPos = true;
		}
		updateDrawer = true;
	}
	if (fractalInfo.useCustomJulPos)
	{
		ImGui::InputDouble("Custom Position X", &fractalInfo.CustomJulPosX, 0.0, 0.0, " % .16f");
		ImGui::InputDouble("Custom Position Y", &fractalInfo.CustomJulPosY, 0.0, 0.0, " % .16f");
	}
	if (FractalDictionary::GetInfo(fractalDrawer->GetFractalType()).extraValues >= 2)
	{ 
		ImGui::Text("Additional Constant (set with V):");
		ImGui::InputDouble("X", &fractalInfo.DistortionVectorX, 0.0, 0.0, " % .16f");
		ImGui::InputDouble("Y", &fractalInfo.DistortionVectorY, 0.0, 0.0, " % .16f");
	}
	if (ImGui::Button("Update"))
	{
		fractalDrawer->SetIterations(fractalInfo.iterations);
		fractalDrawer->SetMinDeviation(fractalInfo.minDeviation);
		fractalDrawer->SetDeviationCycles(fractalInfo.deviationCycles, fractalInfo.debugDeviations);
		fractalDrawer->SetLengthLimit(fractalInfo.lengthLimit);
		fractalDrawer->SetFractalType(fractalInfo.type);
		updateDrawer = true;
		// We need to regenerate the pixel buffer due to changed texture dimensions in case upscale has changed
		regenBuffer = true;
	}
	if (ImGui::Button("Reset Zoom"))
	{
		zoomTransform = DEFAULT_ZOOMTRANSFORM;
		smoothZoomer.EndZoom();
		smoothZoomer.SyncTransforms(zoomTransform);
		updateDrawer = true;
	}
	if (ImGui::Button("Choose Ramp Texture"))
	{
		rampTexFileBrowser.Open();
	}
	DisplayToolTip("Change the ramp texture to a PNG on your hard drive");
	ImGui::SameLine();
	if (ImGui::Button("Default"))
	{
		GLuint rampTexture = LoadRampTexture();
		fractalInterpreter.SetRampTexture(rampTexture);
		glDeleteTextures(1, &rampTexture);
		updateInterpreter = true;
	}
	DisplayToolTip("Use the default ramp texture");
	ImGui::Checkbox("Show advanced options", &fractalInfo.showAdvancedOptions);
	if (fractalInfo.showAdvancedOptions)
	{
		ImGui::Text("Set this value to something small to improve rendering time");
		ImGui::InputDouble("Minimum Deviation", &fractalInfo.minDeviation, SMALL_DOUBLE_VALUE, 0.0, "%.15f");
		ImGui::InputInt("Deviation Cycles", &fractalInfo.deviationCycles);
		ImGui::Checkbox("Show Deviation Iterations", &fractalInfo.debugDeviations);
	}

	ImGui::Checkbox("Live Rendering", &fractalInfo.liveUpdate);
	ImGui::ProgressBar(fractalDrawer->GetProgress());
	ImGui::ProgressBar(fractalInterpreter.GetProgress());

	//debug stuff

	ImGui::End();
	rampTexFileBrowser.SetWindowSize(UI_WINDOW_WIDTH, UI_WINDOW_HEIGHT);
	ImGui::SetWindowPos(ImVec2(0, 0));
	rampTexFileBrowser.Display();
	if (rampTexFileBrowser.HasSelected())
	{
		GLuint rampTexture = LoadRampTexture(rampTexFileBrowser.GetSelected().string());
		fractalInterpreter.SetRampTexture(rampTexture);
		glDeleteTextures(1, &rampTexture);
		rampTexFileBrowser.ClearSelected();
		updateInterpreter = true;
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
	// Create extra values matrix
	int numExtraValues = FractalDictionary::GetMaxValues();
	numExtraValues = max(numExtraValues, 1);
	ComplexFloat* extraValues = new ComplexFloat[numExtraValues];
	// initialize fractal drawer class
	FractalDrawer* fractalDrawer = new FractalDrawer(currentWindowWidth, currentWindowHeight, numExtraValues);
	// create fractal interpreter
	FractalInterpreter fractalInterpreter;
	// current zoom level
	ZoomTransform currentZoom = DEFAULT_ZOOMTRANSFORM;
	// create smooth zoomer
	FractalSmoothZoomer smoothZoomer;
	smoothZoomer.SyncTransforms(currentZoom);
	// initialize quad drawer class
	QuadDrawer quadDrawer;

	//load and set ramp texture
	GLuint rampTexture = LoadRampTexture();
	fractalInterpreter.SetRampTexture(rampTexture);
	glDeleteTextures(1, &rampTexture);
	//initial delta time start
	auto deltaTimeStart = std::chrono::high_resolution_clock::now();
	double totalTime = 0;
	bool animateFractal = true;
	bool SpaceBarPressed = false;
	bool juliaPosUpdate = false;
	bool updateDrawer = false;
	bool updateInterpreter = false;
	bool regenBuffer = false;
	float zoomProgress = 0;
	float zoomStartProgress = 0;
	glm::vec4 windowTransform = glm::vec4(0, 0, 1, 1);
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
	fractalDrawer->SetMinDeviation(fracInfo.minDeviation);
	fractalDrawer->SetLengthLimit(fracInfo.lengthLimit);
	fractalDrawer->SetFractalType(fracInfo.type);

	//file dialog
	ImGui::FileBrowser rampTexFileDialog;
	rampTexFileDialog.SetTitle("Custom Ramp Texture");
	rampTexFileDialog.SetTypeFilters({ ".png" });

	bool firstDraw = true;
	while (!glfwWindowShouldClose(window) && !glfwWindowShouldClose(uiWindow))
	{
		glfwMakeContextCurrent(window);
		// calculate delta time
		auto deltaTimeEnd = std::chrono::high_resolution_clock::now();
		double deltaTime = std::chrono::duration_cast<std::chrono::nanoseconds>(deltaTimeEnd - deltaTimeStart).count() / 1000000000.0;
		deltaTimeStart = std::chrono::high_resolution_clock::now();
		if (fracInfo.animate) totalTime += deltaTime;
		// Check if any events have been activiated (key pressed, 
		//mouse moved etc.) and call corresponding response functions 
		glfwPollEvents();

		//zoom?
		int leftMBstate = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
		int rightMBstate = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
		int middleMBstate = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);
		int JKeyState = glfwGetKey(window, GLFW_KEY_J);
		int JKeyState2 = glfwGetKey(uiWindow, GLFW_KEY_J);
		int DKeyState = glfwGetKey(window, GLFW_KEY_V);
		int DKeyState2 = glfwGetKey(uiWindow, GLFW_KEY_V);
		double mbxpos, mbypos;
		glfwGetCursorPos(window, &mbxpos, &mbypos);
		mbxpos /= currentWindowWidth;
		mbypos /= currentWindowHeight;
		bool didZoom = false;
		if (leftMBstate == GLFW_PRESS)
		{
			currentZoom.ZoomInOrOut(mbxpos, 1 - mbypos, pow(1.0 / ZOOM_PER_SECOND, deltaTime), (float)currentWindowWidth / currentWindowHeight);
			didZoom = true;
		}
		else if (rightMBstate == GLFW_PRESS)
		{
			currentZoom.ZoomInOrOut(mbxpos, 1 - mbypos, pow(ZOOM_PER_SECOND, deltaTime), (float)currentWindowWidth / currentWindowHeight);
			didZoom = true;
		}

		bool chooseJuliaValue = middleMBstate == GLFW_PRESS || JKeyState == GLFW_PRESS || JKeyState2 == GLFW_PRESS;
		if (chooseJuliaValue)
		{
			ComplexFloat newPos = currentZoom.ScreenToWorldPos(mbxpos, 1 - mbypos, (float)currentWindowWidth / currentWindowHeight);
			fracInfo.CustomJulPosX = newPos.real;
			fracInfo.CustomJulPosY = newPos.imaginary;
			fracInfo.useCustomJulPos = true;
			fracInfo.animate = false;
		}
		juliaPosUpdate = chooseJuliaValue;

		bool chooseDirectionValue = DKeyState == GLFW_PRESS || DKeyState2 == GLFW_PRESS;
		if (chooseDirectionValue && numExtraValues >= 2)
		{
			ComplexFloat newPos = currentZoom.ScreenToWorldPos(mbxpos, 1 - mbypos, (float)currentWindowWidth / currentWindowHeight);
			fracInfo.DistortionVectorX = newPos.real;
			fracInfo.DistortionVectorY = newPos.imaginary;
		}
		extraValues[1] = ComplexFloat(fracInfo.DistortionVectorX, fracInfo.DistortionVectorY);

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
		fractalInterpreter.period = fracInfo.period;
		fractalInterpreter.offset = fracInfo.offset;


		// Draw fractal
		bool liveUpdate = fracInfo.liveUpdate && !smoothZoomer.IsZooming() && !smoothZoomer.IsZoomReady();
		bool interpreterDrew = false;
		bool updateOnExtraValueChange = (fracInfo.animate || juliaPosUpdate) && FractalDictionary::GetInfo(fractalDrawer->GetFractalType()).extraValues > 0;
		updateOnExtraValueChange = updateOnExtraValueChange || chooseDirectionValue && FractalDictionary::GetInfo(fractalDrawer->GetFractalType()).extraValues > 1;
		bool zoomMismatch = currentZoom.scale != fractalDrawer->GetRenderedZoom().scale;
		bool shouldStartDrawing = (updateOnResize || updateOnExtraValueChange || updateDrawer || zoomMismatch || didZoom || firstDraw);
		bool shouldSetupZoomer = shouldStartDrawing;
		shouldStartDrawing = shouldStartDrawing && !fractalInterpreter.IsBusy();
		firstDraw = false;
		if (shouldSetupZoomer)
		{
			smoothZoomer.SetupZoom(currentZoom);
		}
		extraValues[0] = ComplexFloat(fracInfo.CustomJulPosX, fracInfo.CustomJulPosY);
		if (!fracInfo.useCustomJulPos)
		{
			extraValues[0] = ComplexFloat((cos(totalTime) * 0.5 - cos(totalTime * 2) * 0.25) * 1.01,
				(sin(totalTime) * 0.5 - sin(totalTime * 2) * 0.25) * 1.01);
		}
		bool fractalDrawerReady = fractalDrawer->Draw(shouldStartDrawing, currentZoom, extraValues, fracInfo.power);
		bool shouldRenderInterpreter = fractalDrawerReady; //render if the fractal drawer finished this frame;
		shouldRenderInterpreter = shouldRenderInterpreter || (liveUpdate && fractalDrawer->IsBusy()); // render if the fractalDrawer is busy if liveupdate is enabled
		shouldRenderInterpreter = shouldRenderInterpreter || updateInterpreter; // render if we've change a UI thing that affects the interpreter
		//shouldRenderInterpreter = shouldRenderInterpreter && !fractalInterpreter.IsBusy(); // don't render the interpreter if it's already busy
		if (shouldRenderInterpreter)
		{
			int fractalWidth = 0;
			int fractalHeight = 0;
			fractalDrawer->GetBufferDimensions(fractalWidth, fractalHeight);
			fractalInterpreter.CreateOrUpdateBuffers(fractalWidth, fractalHeight);
			fractalDrawer->CopyBuffer(fractalInterpreter.GetValueBufferStart(), fractalWidth * fractalHeight * sizeof(CF_Float));
		}
		bool interpreterReady = fractalInterpreter.Draw(shouldRenderInterpreter);
		int interpreterWidth = 0;
		int interpreterHeight = 0;
		const float* interpreterColors = fractalInterpreter.GetColors(interpreterWidth, interpreterHeight);

		if (interpreterReady) {
			if (smoothZoomer.IsZooming())
			{
				smoothZoomer.EndZoom();
			}
			if (smoothZoomer.IsZoomReady() && !smoothZoomer.IsZooming()) // assume live update is disabled
			{
				smoothZoomer.StartZoom();
			}
		}
		else
		{
			if (!fractalInterpreter.IsBusy() && !fractalDrawer->IsBusy() && smoothZoomer.IsZooming()) // Stop bug where zoom will start when everything is done
			{
				smoothZoomer.EndZoom();
			}
		}
		smoothZoomer.RunProgressLogic(fractalDrawer->GetProgress(), fractalInterpreter.GetProgress(), fractalInterpreter.GetInterpreterTime());

		windowTransform = smoothZoomer.GetBoundMults((float)currentWindowHeight / currentWindowWidth);
		quadDrawer.DrawBuffer(window, interpreterColors, GL_RGB, interpreterWidth, interpreterHeight, 
			windowTransform.x * currentWindowWidth, windowTransform.y * currentWindowHeight, 
			windowTransform.z * currentWindowWidth, windowTransform.w * currentWindowHeight, 
			fractalDrawer->GetMipLevel(), interpreterReady);
		glfwSwapBuffers(window);

		updateDrawer = false;
		updateInterpreter = false;
		regenBuffer = false;
		//Render IMGUI stuff
		RenderUIWindow(uiWindow, fractalDrawer, updateDrawer, updateInterpreter, regenBuffer, fracInfo, fractalInterpreter, smoothZoomer, currentZoom, rampTexFileDialog);
	}
	delete fractalDrawer;
	delete[] extraValues;
	glfwTerminate();

	return EXIT_SUCCESS;
}
