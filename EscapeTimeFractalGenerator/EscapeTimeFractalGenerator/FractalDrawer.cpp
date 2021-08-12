#include "FractalDrawer.h"
#include "ComplexFractal.h"
#include <math.h>;

std::mutex mtx;
const glm::vec3 STARTING_TRANSFORM = glm::vec3(0.5, 0.5, 3);
const int NUM_ITERATIONS = 30;
const float VALUE_POWER = 0.4;

FractalDrawer::FractalDrawer(int width, int height, GLFWwindow* window) 
{
	this->pixelBufferHeight = height;
	this->pixelBufferWidth = width;
	this->window = window;
	pixelBuffer = new float[width * height * 3];
	glGenTextures(1, &fractalTexture);
	transform = STARTING_TRANSFORM;
	startTime = high_resolution_clock::now();
}

FractalDrawer::~FractalDrawer()
{
		delete[] pixelBuffer;
		if (rampColors != nullptr)
		{
			delete[] rampColors;
		}
		glDeleteTextures(1, &fractalTexture);
}


void FractalDrawer::SetRampTexture(GLuint textureID)
{
	rampTexture = textureID;
	// Build Ramp Color Array
	if (rampTexture != 0)
	{
		glBindTexture(GL_TEXTURE_2D, rampTexture);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &ramTexWidth);
		rampColors = new float[ramTexWidth * 3];
		glGetnTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, ramTexWidth * 3 * sizeof(float), rampColors);
	}
}

void FractalDrawer::DrawPixel(float* pixelBuffer, int pixelBufferWidth, int pixelBufferHeight, int x, int y, float r, float g, float b)
{
	if (x < 0 || x >= pixelBufferWidth || y < 0 || y >= pixelBufferHeight) return;
	int startIndex = 3 * (y * pixelBufferWidth + x);
	pixelBuffer[startIndex] = r;
	pixelBuffer[startIndex + 1] = g;
	pixelBuffer[startIndex + 2] = b;
}

bool FractalDrawer::DrawFractalChunk(float* pixelBuffer, int pixelBufferWidth, int pixelBufferHeight, int ystart, int yend, const float* rampColors, int rampColorsWidth, glm::vec3 transform, float time, std::atomic_bool& halt)
{
	ComplexFractal fractal = ComplexFractal(NUM_ITERATIONS);
	fractal.SetStartingFunction([](ComplexFloat input, float time) {return input; });
	fractal.SetFunction([](ComplexFloat input, ComplexFloat previousValue, float time) {return previousValue * previousValue + ComplexFloat(sin(time), cos(time * 0.7853975)); });
	for (int i = ystart; i < yend; i++)
	{
		for (int j = 0; j < pixelBufferWidth; j++)
		{
			if (halt)
			{
				halt = false;
				return false;
			}
			float x = (float)j / pixelBufferWidth;
			float y = (float)i / pixelBufferHeight;
			x = (x - transform.x) * transform.z * pixelBufferWidth / pixelBufferHeight;
			y = (y - transform.y) * transform.z;
			float value = fractal.CalculateEscapeTime(x, y, time);
			if (value == 0)
			{
				DrawPixel(pixelBuffer, pixelBufferWidth, pixelBufferHeight, j, i, 0, 0, 0);
			}
			else
			{
				float newValue = pow(value, VALUE_POWER);
				if (rampColors != nullptr)
				{
					int rampIndex = ((int)((float)rampColorsWidth * (1 - newValue)));
					if (rampIndex > rampColorsWidth - 1) rampIndex = rampColorsWidth - 1;
					rampIndex *= 3;
					DrawPixel(pixelBuffer, pixelBufferWidth, pixelBufferHeight, j, i, rampColors[rampIndex], rampColors[rampIndex + 1], rampColors[rampIndex + 2]);
					//DrawPixel(pixelBuffer, pixelBufferWidth, pixelBufferHeight, j, i, value, value, value);
				}
				else
					DrawPixel(pixelBuffer, pixelBufferWidth, pixelBufferHeight, j, i, newValue, newValue, newValue);
			}
		}
	}
	return true;
}

bool FractalDrawer::DrawFractal(float* pixelBuffer, int pixelBufferWidth, int pixelBufferHeight, const float* rampColors, int rampColorsWidth, glm::vec3 transform, float time, std::atomic_bool &halt)
{
	std::lock_guard<std::mutex> lock1{ mtx };
	std::cout << "drawing fractal" << std::endl;
	ComplexFractal fractal = ComplexFractal(NUM_ITERATIONS);
	fractal.SetStartingFunction([](ComplexFloat input, float time) {return input; });
	fractal.SetFunction([](ComplexFloat input, ComplexFloat previousValue, float time) {return previousValue * previousValue + ComplexFloat(sin(time), cos(time * 0.7853975)); });

	
	for (int i = 0; i < pixelBufferHeight; i++)
	{
		for (int j = 0; j < pixelBufferWidth; j++)
		{
			if (halt)
			{
				halt = false;
				return false;
			}
			float x = (float)j / pixelBufferWidth;
			float y = (float)i / pixelBufferHeight;
			x = (x - transform.x) * transform.z * pixelBufferWidth / pixelBufferHeight;
			y = (y - transform.y) * transform.z;
			float value = fractal.CalculateEscapeTime(x, y, time);
			if (value == 0)
			{
				DrawPixel(pixelBuffer, pixelBufferWidth, pixelBufferHeight, j, i, 0, 0, 0);
			}
			else
			{
				float newValue = pow(value, VALUE_POWER);
				if (rampColors != nullptr)
				{
					int rampIndex = ((int)((float)rampColorsWidth * (1 - newValue)));
					if (rampIndex > rampColorsWidth - 1) rampIndex = rampColorsWidth - 1;
					rampIndex *= 3;
					DrawPixel(pixelBuffer, pixelBufferWidth, pixelBufferHeight, j, i, rampColors[rampIndex], rampColors[rampIndex + 1], rampColors[rampIndex + 2]);
					//DrawPixel(pixelBuffer, pixelBufferWidth, pixelBufferHeight, j, i, value, value, value);
				}
				else
					DrawPixel(pixelBuffer, pixelBufferWidth, pixelBufferHeight, j, i, newValue, newValue, newValue);
			}
		}
	}
	return true;
}

void FractalDrawer::Resize(int width, int height)
{
	if (drawFractalThread.valid())
	{
		haltDrawingThread = true;
		drawFractalThread.wait();
	}
	mtx.lock();
	delete[] pixelBuffer;
	pixelBuffer = new float[width * height * 3];
	pixelBufferWidth = width;
	pixelBufferHeight = height;
	mtx.unlock();
}

bool FractalDrawer::Draw(bool update)
{
	fractalThreadNeedsRun = fractalThreadNeedsRun || update;
	//Draw Fractal
	bool anyThreadsValid = false;
	for (int i = 0; i < NUM_FRACTAL_DRAW_THREADS; i++)
	{
		//any threads are valid
		anyThreadsValid = anyThreadsValid || drawFractalThreads[i].valid();
	}
	if (!anyThreadsValid && fractalThreadNeedsRun)
	{
		steady_clock::time_point currentTime = high_resolution_clock::now();
		float time = duration_cast<milliseconds>(currentTime - startTime).count() * 0.001;
		//drawFractalThread = std::async(std::launch::async, &DrawFractal, pixelBuffer, pixelBufferWidth, pixelBufferHeight, rampColors, ramTexWidth, transform, time, std::ref(haltDrawingThread));
		for (int i = 0; i < NUM_FRACTAL_DRAW_THREADS; i++)
		{
			int chunksize = pixelBufferHeight / 16; //needs to be 15 to get all pixels
			int ystart = i * chunksize;
			int yend = i == 15 ? pixelBufferHeight : (i + 1) * chunksize;
			drawFractalThreads[i] = std::async(std::launch::async, &DrawFractalChunk, pixelBuffer, pixelBufferWidth, pixelBufferHeight, ystart, yend, rampColors, ramTexWidth, transform, time, std::ref(haltDrawingThread));
		}
		fractalThreadNeedsRun = false;
	}
	//DrawFractal(currentPixelBuffer, pixelBufferWidth, pixelBufferHeight, rampColors, ramTexWidth, glm::vec3(0.5, 0.5, 2));
	
	glBindTexture(GL_TEXTURE_2D, fractalTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	std::future_status drawingStatus[NUM_FRACTAL_DRAW_THREADS];
	bool allThreadsValid = true;
	bool allThreadsReady = true;
	for (int i = 0; i < NUM_FRACTAL_DRAW_THREADS; i++)
	{
		//any threads are valid
		allThreadsValid = allThreadsValid && drawFractalThreads[i].valid();
	}
	if (allThreadsValid)
	{
		for (int i = 0; i < NUM_FRACTAL_DRAW_THREADS; i++)
		{
			drawingStatus[i] = drawFractalThreads[i].wait_for(std::chrono::seconds(0));
			allThreadsReady = allThreadsReady || (drawingStatus[i] == std::future_status::ready);
		}
		if (allThreadsReady)
		{
			for (int i = 0; i < NUM_FRACTAL_DRAW_THREADS; i++)
			{
				drawFractalThreads[i].get();
			}
			mtx.lock();
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pixelBufferWidth, pixelBufferHeight, 0, GL_RGB, GL_FLOAT, pixelBuffer);
			mtx.unlock();
		}
	}

	GLuint fbo = 0;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, fractalTexture, 0);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	int width = 0;
	int height = 0;
	glfwGetWindowSize(window, &width, &height);
	glBlitFramebuffer(0, 0, pixelBufferWidth, pixelBufferHeight, 0, 0, width, height,
		GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glDeleteFramebuffers(1, &fbo);
	return true;
}