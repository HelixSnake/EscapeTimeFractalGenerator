#include "FractalDrawer.h"
#include "ComplexFractal.h"
#include <math.h>;
#include <glm/vec2.hpp>;
#include <glm/common.hpp>;

std::mutex mtx;
const glm::vec3 STARTING_TRANSFORM = glm::vec3(0.5, 0.5, 3);
//const float VALUE_POWER = 0.4;
//const float VALUE_MULT = 0.01;
//const float VALUE_PERIOD = 100;
//const float LENGTH_LIMIT = 100;

FractalDrawer::FractalDrawer(int width, int height, GLFWwindow* window) 
{
	this->pixelBufferHeight = height;
	this->pixelBufferWidth = width;
	this->window = window;
	pixelBuffer = new float[width * height * 3];
	glGenTextures(1, &fractalTexture);
	transformx = STARTING_TRANSFORM.x;
	transformy = STARTING_TRANSFORM.y;
	transformz = STARTING_TRANSFORM.z;
	lastTransformx = transformx;
	lastTransformy = transformy;
	lastTransformz = transformz;
	lastLastTransformx = transformx;
	lastLastTransformy = transformy;
	lastLastTransformz = transformz;
	totalTime = 0;
	lastTime = high_resolution_clock::now();
}

FractalDrawer::~FractalDrawer()
{
	haltDrawingThread = true;
	LockAllMutexes();
	delete[] pixelBuffer;
	if (rampColors != nullptr)
	{
		delete[] rampColors;
	}
	glDeleteTextures(1, &fractalTexture);
	UnlockAllMutexes();
}


void FractalDrawer::SetRampTexture(GLuint textureID)
{
	LockAllMutexes();
	rampTexture = textureID;
	// Build Ramp Color Array
	if (rampTexture != 0)
	{
		glBindTexture(GL_TEXTURE_2D, rampTexture);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &ramTexWidth);
		rampColors = new float[ramTexWidth * 3];
		glGetnTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, ramTexWidth * 3 * sizeof(float), rampColors);
	}
	UnlockAllMutexes();
}

void FractalDrawer::LockAllMutexes() 
{
	mtx.lock();
	for (int i = 0; i < NUM_FRACTAL_DRAW_THREADS; i++)
	{
		Mutexes[i].lock();
	}
}
void FractalDrawer::UnlockAllMutexes()
{
	mtx.unlock();
	for (int i = 0; i < NUM_FRACTAL_DRAW_THREADS; i++)
	{
		Mutexes[i].unlock();
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

bool FractalDrawer::DrawFractalChunk(int index, ComplexFloat extraValue, CF_Float tfx, CF_Float tfy, CF_Float tfscale)
{
	std::lock_guard<std::mutex> lock1{ Mutexes[index] };
	ComplexFractal fractal = ComplexFractal(iterations, minDeviation);
	fractal.lengthLimit = lengthLimit;
	if (currentFractal == FractalType::Julia)
	{
		fractal.SetStartingFunction(FRACTAL_STARTING_FUNCTION_JULIA);
		fractal.SetFunction(FRACTAL_RECURSIVE_FUNCTION_JULIA);
	}
	else
	{
		fractal.SetStartingFunction(FRACTAL_STARTING_FUNCTION_MANDEL);
		fractal.SetFunction(FRACTAL_RECURSIVE_FUNCTION_MANDEL);
	}
	int currentThreadProgress = 0;
	for (int i = index; i < pixelBufferHeight; i += NUM_FRACTAL_DRAW_THREADS)
	{
		for (int j = 0; j < pixelBufferWidth; j++)
		{
			threadProgress[index] = currentThreadProgress;
			if (haltDrawingThread)
			{
				return false;
			}
			CF_Float x = (CF_Float)j / pixelBufferWidth;
			CF_Float y = (CF_Float)i / pixelBufferHeight;
			x = (x * pixelBufferWidth / pixelBufferHeight - tfx) * tfscale;
			y = (y - tfy) * tfscale;
			CF_Float value = fractal.CalculateEscapeTime(x, y, extraValue);
			if (value == 0)
			{
				DrawPixel(pixelBuffer, pixelBufferWidth, pixelBufferHeight, j, i, 0, 0, 0);
			}
			else
			{
				//logarithmic function keeps period similar at all zoom levels
				float newValue = glm::fract(log(value) / period);
				//float newValue = glm::fract(value / period);
				if (rampColors != nullptr)
				{
					int rampIndex = ((int)((float)ramTexWidth * (1 - newValue)));
					if (rampIndex > ramTexWidth - 1) rampIndex = ramTexWidth - 1;
					if (rampIndex < 0) rampIndex = 0;
					rampIndex *= 3;
					DrawPixel(pixelBuffer, pixelBufferWidth, pixelBufferHeight, j, i, rampColors[rampIndex], rampColors[rampIndex + 1], rampColors[rampIndex + 2]);
					//DrawPixel(pixelBuffer, pixelBufferWidth, pixelBufferHeight, j, i, value, value, value);
				}
				else
					DrawPixel(pixelBuffer, pixelBufferWidth, pixelBufferHeight, j, i, newValue, newValue, newValue);
			}
			currentThreadProgress++;
		}
	}
	return true;
}

//unused legacy code, may clean up later
/*
bool FractalDrawer::DrawFractal(float* pixelBuffer, int pixelBufferWidth, int pixelBufferHeight, const float* rampColors, int rampColorsWidth, glm::vec3 transform, float time, std::atomic_bool &halt)
{
	std::lock_guard<std::mutex> lock1{ mtx };
	std::cout << "drawing fractal" << std::endl;
	ComplexFractal fractal = ComplexFractal(iterations);
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
			CF_Float x = (CF_Float)j / pixelBufferWidth;
			CF_Float y = (CF_Float)i / pixelBufferHeight;
			CF_Float ratio = (CF_Float)pixelBufferWidth / pixelBufferHeight;
			x = (x - (CF_Float)transform.x) * (CF_Float)transform.z * ratio;
			y = (y - (CF_Float)transform.y) * (CF_Float)transform.z;
			CF_Float value = fractal.CalculateEscapeTime(x, y, time);
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
}*/

void FractalDrawer::Resize(int width, int height, float sizeMult)
{
	haltDrawingThread = true;
	LockAllMutexes();
	delete[] pixelBuffer;
	pixelBuffer = new float[width * sizeMult * height * sizeMult * 3];
	pixelBufferWidth = width * sizeMult;
	pixelBufferHeight = height *sizeMult;
	upScale = sizeMult;
	UnlockAllMutexes();
	haltDrawingThread = false;
}
void FractalDrawer::SetIterations(int iterations)
{
	LockAllMutexes();
	this->iterations = iterations;
	UnlockAllMutexes();
}

void FractalDrawer::SetPeriod(float period)
{
	LockAllMutexes();
	this->period = period;
	UnlockAllMutexes();
}

void FractalDrawer::SetMinDeviation(float minDeviation)
{
	LockAllMutexes();
	this->minDeviation = minDeviation;
	UnlockAllMutexes();
}

void FractalDrawer::SetLengthLimit(float lengthLimit)
{
	LockAllMutexes();
	this->lengthLimit = lengthLimit;
	UnlockAllMutexes();
}

void FractalDrawer::SetFractal(FractalType fractal)
{
	LockAllMutexes();
	this->currentFractal = fractal;
	UnlockAllMutexes();
}

void FractalDrawer::SetCustomJuliaPosition(bool use, double x, double y)
{
	// no need to lock mutexes as these values are not used by the fractal drawing thread function
	useCustomJuliaPosition = use;
	customJuliaPosition = ComplexFloat(x, y);
}

float FractalDrawer::GetProgress()
{
	return drawingProgress;
}

void FractalDrawer::Zoom(float x, float y, float amount)
{
	//convert x y point to "World space" by replicating transform performed on pixels
	CF_Float newX = (x * pixelBufferWidth / pixelBufferHeight - transformx) * transformz;
	CF_Float newY = (y - transformy) * transformz;
	transformz *= amount;
	transformx -= (newX / transformz) * (1 - amount);
	transformy -= (newY / transformz) * (1 - amount);
}

ComplexFloat FractalDrawer::ScreenToWorldPos(float x, float y)
{
	CF_Float newX = (x * pixelBufferWidth / pixelBufferHeight - transformx) * transformz;
	CF_Float newY = (y - transformy) * transformz;
	return ComplexFloat(newX, newY);
}

bool FractalDrawer::Draw(bool update)
{
	fractalThreadNeedsRun = fractalThreadNeedsRun || update || lastTransformz != transformz;
	//Draw Fractal
	bool anyThreadsValid = false;
	for (int i = 0; i < NUM_FRACTAL_DRAW_THREADS; i++)
	{
		//any threads are valid
		anyThreadsValid = anyThreadsValid || drawFractalThreads[i].valid();
	}
	if (!anyThreadsValid && fractalThreadNeedsRun)
	{
		if (enableAnimation)
		{
			steady_clock::time_point currentTime = high_resolution_clock::now();
			totalTime = totalTime + duration_cast<nanoseconds>(currentTime - lastTime).count() / 1000000000.0;
			lastTime = currentTime;
		}
		for (int i = 0; i < NUM_FRACTAL_DRAW_THREADS; i++)
		{
			int chunksize = pixelBufferHeight / 16;
			int ystart = i * chunksize;
			int yend = i == 15 ? pixelBufferHeight : (i + 1) * chunksize;
			threadProgress[i] = 0;
			ComplexFloat extraValue = customJuliaPosition;
			if (!useCustomJuliaPosition)
			{
				extraValue = ComplexFloat((cos(totalTime) * 0.5 - cos(totalTime * 2) * 0.25) * 1.01,
					(sin(totalTime) * 0.5 - sin(totalTime * 2) * 0.25) * 1.01);
			}
			drawFractalThreads[i] = std::async(std::launch::async, &FractalDrawer::DrawFractalChunk, this, i, extraValue, transformx, transformy, transformz);
		}
		fractalThreadNeedsRun = false;
	}
	if (!enableAnimation)
	{
		lastTime = high_resolution_clock::now();
	}

	glBindTexture(GL_TEXTURE_2D, fractalTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	std::future_status drawingStatus[NUM_FRACTAL_DRAW_THREADS];
	bool allThreadsValid = true;
	bool allThreadsReady = true;
	bool renderedThisFrame = false;
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
			allThreadsReady = allThreadsReady && (drawingStatus[i] == std::future_status::ready);
		}
		if (allThreadsReady)
		{
			for (int i = 0; i < NUM_FRACTAL_DRAW_THREADS; i++)
			{
				drawFractalThreads[i].get();
			}
			LockAllMutexes();
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pixelBufferWidth, pixelBufferHeight, 0, GL_RGB, GL_FLOAT, pixelBuffer);
			lastLastTransformx = lastTransformx;
			lastLastTransformy = lastTransformy;
			lastLastTransformz = lastTransformz;
			lastTransformx = transformx;
			lastTransformy = transformy;
			lastTransformz = transformz;
			renderedThisFrame = true;
			UnlockAllMutexes();
		}
	}

	GLuint fbo = 0;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
	int mipLevel = (int)glm::max(log2(upScale), 0.0f);
	if (mipLevel != 0)
	{
		glGenerateTextureMipmap(fractalTexture);
	}
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, fractalTexture, mipLevel);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	int width = 0;
	int height = 0;
	glfwGetWindowSize(window, &width, &height);

	float avgThreadProgress = 0;
	for (int i = 0; i < NUM_FRACTAL_DRAW_THREADS; i++)
	{
		avgThreadProgress += threadProgress[i];
	}
	avgThreadProgress /= pixelBufferHeight * pixelBufferWidth;
	//std::cout << avgThreadProgress << std::endl;
	if (renderedThisFrame == true) avgThreadProgress = 0;
	//avgThreadProgress = 0;
	drawingProgress = avgThreadProgress;

	CF_Float scaleDiff = lastLastTransformz / lastTransformz;
	CF_Float rendRectX1 = 0;
	CF_Float rendRectX2 = 1;
	CF_Float rendRectY1 = 0;
	CF_Float rendRectY2 = 1;
	CF_Float rendRectNewX1 = 0;
	CF_Float rendRectNewX2 = 1;
	CF_Float rendRectNewY1 = 0;
	CF_Float rendRectNewY2 = 1;
	//guide: oldTX = old transform X, oldTZ = old transform Z(scale), newTX = new transform X, newTZ = new transform Z(scale), pBW & pBH = pixel buffer width and height
	//oldCX = corner before transformation, newCX = corner after transformation (this is the value we're trying to find)
	//x is the position on screen of the cursor, ranging from 0 to 1 horizontally and vertically
	//invert zoom function:
	//newTX = oldTX - ((x * pBW / pBH - oldTX) * newTZ / oldTZ) * (1 - oldTZ / newTZ);
	//solve for x
	//x = ((oldTX - newTX) / (1 - oldTZ / newTZ) * oldTZ / newTZ + oldTX) / pBW * pBH;
	//unfortunately, when 1 - oldTX / newTZ = 0, we have a divide by 0 error; we could check for that, but the proper solution is to optimize that division out of the equation
	//scale to point formula: 
	//newCX = (oldCX - x) * oldTZ / newTZ + x;
	//simplify so x only shows up once: 
	//newCX = (oldCX * oldTZ / newTZ) + x * (1 - oldTZ / newTZ);
	//combine:
	//newCX = (oldCX * oldTZ / newTZ) + (((oldTX - newTX) / (1 - oldTZ / newTZ) * oldTZ / newTZ + oldTX) / pBW * pBH) * (1 - oldTZ / newTZ);
	//newCX = (oldCX * oldTZ / newTZ) + (((oldTX - newTX) / (1 - oldTZ / newTZ) * oldTZ / newTZ * (1 - oldTZ / newTZ) + oldTX * (1 - oldTZ / newTZ)) / pBW * pBH);
	//newCX = (oldCX * oldTZ / newTZ) + (((oldTX - newTX) * oldTZ / newTZ + oldTX * (1 - oldTZ / newTZ)) / pBW * pBH);
	//for Y it's the same equation, except you don't need pBW or pBH and all instances of X are replaced with Y.
	if (scaleDiff != 1)
	{
		rendRectNewX1 = rendRectX1 * scaleDiff + (lastTransformx - lastLastTransformx + (lastLastTransformx * (1 - scaleDiff))) / pixelBufferWidth * pixelBufferHeight;
		rendRectNewY1 = rendRectY1 * scaleDiff + lastTransformy - lastLastTransformy + (lastLastTransformy * (1 - scaleDiff));
		rendRectNewX2 = rendRectX2 * scaleDiff + (lastTransformx - lastLastTransformx + (lastLastTransformx * (1 - scaleDiff))) / pixelBufferWidth * pixelBufferHeight;
		rendRectNewY2 = rendRectY2 * scaleDiff + lastTransformy - lastLastTransformy + (lastLastTransformy * (1 - scaleDiff));
	}
	rendRectX1 = glm::mix(rendRectX1, rendRectNewX1, avgThreadProgress);
	rendRectX2 = glm::mix(rendRectX2, rendRectNewX2, avgThreadProgress);
	rendRectY1 = glm::mix(rendRectY1, rendRectNewY1, avgThreadProgress);
	rendRectY2 = glm::mix(rendRectY2, rendRectNewY2, avgThreadProgress);
	glClearColor(clearColor.x, clearColor.y, clearColor.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	float frameBufferScale = pow(2, mipLevel);
	glBlitFramebuffer(0, 0, pixelBufferWidth / frameBufferScale, pixelBufferHeight / frameBufferScale, rendRectX1 * width, rendRectY1 * height, rendRectX2 * width, rendRectY2 * height,
		GL_COLOR_BUFFER_BIT, GL_LINEAR);
	glDeleteFramebuffers(1, &fbo);
	return true;
}