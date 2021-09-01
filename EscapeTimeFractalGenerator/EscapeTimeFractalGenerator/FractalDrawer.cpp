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

FractalDrawer::FractalDrawer(int width, int height) 
{
	drawFractalThreads = new std::future<bool>[NUM_FRACTAL_DRAW_THREADS];
	threadProgress = new std::atomic<int>[NUM_FRACTAL_DRAW_THREADS];
	Mutexes = new std::mutex[NUM_FRACTAL_DRAW_THREADS];
	drawingStatus = new std::future_status[NUM_FRACTAL_DRAW_THREADS];

	this->pixelBufferHeight = height;
	this->pixelBufferWidth = width;
	pixelBuffer = new std::atomic<CF_Float>[width * height];
	ResetZoom();
	totalTime = 0;
	lastTimeAnim = high_resolution_clock::now();
	lastTimeDelta = high_resolution_clock::now();
}

FractalDrawer::~FractalDrawer()
{
	haltDrawingThread = true;
	LockAllMutexes();
	delete[] pixelBuffer;
	UnlockAllMutexes();
	delete[] drawFractalThreads;
	delete[] drawingStatus;
	delete[] threadProgress;
	delete[] Mutexes;
}


void FractalDrawer::LockAllMutexes(bool haltDrawing)
{
	if (haltDrawing) haltDrawingThread = true;
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

void FractalDrawer::SetPixel(std::atomic<CF_Float>* pixelBuffer, int pixelBufferWidth, int pixelBufferHeight, int x, int y, CF_Float value)
{
	if (x < 0 || x >= pixelBufferWidth || y < 0 || y >= pixelBufferHeight) return;
	pixelBuffer[y * pixelBufferWidth + x] = value;
}

bool FractalDrawer::DrawFractalChunk(int index, ComplexFloat extraValue, CF_Float tfx, CF_Float tfy, CF_Float tfscale)
{
	std::lock_guard<std::mutex> lock1{ Mutexes[index] };
	ComplexFractal fractal = ComplexFractal(iterations, minDeviation, deviationCycles, debugDeviations);
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
			glm::vec2 UV;
			CF_Float value = fractal.CalculateEscapeTime(x, y, extraValue, UV);
			if (value == 0)
			{
				SetPixel(pixelBuffer, pixelBufferWidth, pixelBufferHeight, j, i, 0);
			}
			else
			{
				//float newValue = glm::fract(value / period);
				SetPixel(pixelBuffer, pixelBufferWidth, pixelBufferHeight, j, i, value);
			}
			currentThreadProgress++;
		}
	}
	return true;
}

void FractalDrawer::Resize(int width, int height, double sizeMult)
{
	haltDrawingThread = true;
	LockAllMutexes();
	delete[] pixelBuffer;
	pixelBuffer = new std::atomic<CF_Float>[width * sizeMult * height * sizeMult];
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

void FractalDrawer::SetMinDeviation(double minDeviation)
{
	LockAllMutexes();
	this->minDeviation = minDeviation;
	UnlockAllMutexes();
}

void FractalDrawer::SetDeviationCycles(int deviationCycles, bool debugDeviations)
{
	LockAllMutexes();
	this->deviationCycles = deviationCycles;
	this->debugDeviations = debugDeviations;
	UnlockAllMutexes();
}

void FractalDrawer::SetLengthLimit(double lengthLimit)
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

void FractalDrawer::Zoom(double x, double y, double amount)
{
	if (disableZoom) return;
	//convert x y point to "World space" by replicating transform performed on pixels
	CF_Float newX = (x * pixelBufferWidth / pixelBufferHeight - currentTransform.x) * currentTransform.scale;
	CF_Float newY = (y - currentTransform.y) * currentTransform.scale;
	currentTransform.scale *= amount;
	currentTransform.x -= (newX / currentTransform.scale) * (1 - amount);
	currentTransform.y -= (newY / currentTransform.scale) * (1 - amount);
}
void FractalDrawer::ResetZoom()
{
	haltDrawingThread = true;
	LockAllMutexes();
	currentTransform.x = STARTING_TRANSFORM.x;
	currentTransform.y = STARTING_TRANSFORM.y;
	currentTransform.scale = STARTING_TRANSFORM.z;
	lastTransform = currentTransform;
	lastTransform2 = lastTransform;
	UnlockAllMutexes();
	haltDrawingThread = false;
}

ComplexFloat FractalDrawer::ScreenToWorldPos(double x, double y)
{
	CF_Float newX = (x * pixelBufferWidth / pixelBufferHeight - currentTransform.x) * currentTransform.scale;
	CF_Float newY = (y - currentTransform.y) * currentTransform.scale;
	return ComplexFloat(newX, newY);
}

glm::vec4 FractalDrawer::GetBounds(float progress)
{
	CF_Float scaleDiff = lastTransform2.scale / lastTransform.scale;
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
		rendRectNewX1 = rendRectX1 * scaleDiff + (lastTransform.x - lastTransform2.x + (lastTransform2.x * (1 - scaleDiff))) / pixelBufferWidth * pixelBufferHeight;
		rendRectNewY1 = rendRectY1 * scaleDiff + lastTransform.y - lastTransform2.y + (lastTransform2.y * (1 - scaleDiff));
		rendRectNewX2 = rendRectX2 * scaleDiff + (lastTransform.x - lastTransform2.x + (lastTransform2.x * (1 - scaleDiff))) / pixelBufferWidth * pixelBufferHeight;
		rendRectNewY2 = rendRectY2 * scaleDiff + lastTransform.y - lastTransform2.y + (lastTransform2.y * (1 - scaleDiff));
	}
	rendRectX1 = glm::mix(rendRectX1, rendRectNewX1, progress);
	rendRectX2 = glm::mix(rendRectX2, rendRectNewX2, progress);
	rendRectY1 = glm::mix(rendRectY1, rendRectNewY1, progress);
	rendRectY2 = glm::mix(rendRectY2, rendRectNewY2, progress);
	return glm::vec4(rendRectX1, rendRectY1, rendRectX2, rendRectY2);
}

void FractalDrawer::GetBufferDimensions(int& bufferWidth, int& bufferHeight)
{
	bufferWidth = pixelBufferWidth;
	bufferHeight = pixelBufferHeight;
}

void FractalDrawer::CopyBuffer(CF_Float* dest, size_t bufferSize)
{
	int length = bufferSize / sizeof(CF_Float);
	length = glm::min(length, pixelBufferHeight * pixelBufferWidth);
	for (int i = 0; i < length; i++)
	{
		dest[i] = pixelBuffer[i].load();
	}
}

bool FractalDrawer::Draw(bool update)
{
	bool shouldDraw = false; // Value to return from function when it's OK to draw
	fractalThreadNeedsRun = fractalThreadNeedsRun || update || lastTransform.scale != currentTransform.scale;
	//Draw Fractal
	bool anyThreadsValid = false;
	for (int i = 0; i < NUM_FRACTAL_DRAW_THREADS; i++)
	{
		//any threads are valid
		anyThreadsValid = anyThreadsValid || drawFractalThreads[i].valid();
	}
	if (!anyThreadsValid && fractalThreadNeedsRun)
	{
		//make sure drawing threads are set to not halt
		haltDrawingThread = false;
		if (enableAnimation)
		{
			steady_clock::time_point currentTime = high_resolution_clock::now();
			totalTime = totalTime + duration_cast<nanoseconds>(currentTime - lastTimeAnim).count() / 1000000000.0;
			lastTimeAnim = currentTime;
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
			drawFractalThreads[i] = std::async(std::launch::async, &FractalDrawer::DrawFractalChunk, this, i, extraValue, 
				currentTransform.x, currentTransform.y, currentTransform.scale);
		}
		lastTimeDelta = high_resolution_clock::now();
		fractalThreadNeedsRun = false;
	}
	if (!enableAnimation)
	{
		lastTimeAnim = high_resolution_clock::now();
	}

	steady_clock::time_point currentTimeDelta = high_resolution_clock::now();
	double timeSinceLastRender = duration_cast<nanoseconds>(currentTimeDelta - lastTimeDelta).count() / 1000000000.0;
	disableZoom = false;
	if (timeSinceLastRender > 1) // We don't want the delay on zooming in or out to be more than a second
	{
		if (lastTransform.scale != currentTransform.scale) // we are zooming in or out
		{
			if (lastTransform2.scale == lastTransform.scale) // we have only started zooming in or out
			{
				haltDrawingThread = true;
			}
		}
	}
	if (timeSinceLastRender > 3 && lastTransform.scale != currentTransform.scale && lastTransform2.scale != lastTransform.scale)
	{
		// Stop us from zooming out of control!
		disableZoom = true;
	}

	bool allThreadsValid = true;
	bool allThreadsReady = true;
	bool renderedThisFrame = false;
	for (int i = 0; i < NUM_FRACTAL_DRAW_THREADS; i++)
	{
		//all threads are valid
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
			LockAllMutexes(false);
			shouldDraw = true; // DRAW
			lastTransform2 = lastTransform;
			lastTransform = currentTransform;
			renderedThisFrame = true;
			UnlockAllMutexes();
		}
	}
	//POTENTIAL GLITCHY BEHAVIOR: REMOVE IF THE PROGRAM BREAKS IN ANY WAY
	//pixelBuffer has been moved from float* to std::atomic<float>* so this should be safe now 
	if (lastTransform.scale == lastTransform.scale && lastTransform.scale == currentTransform.scale && liveUpdate && anyThreadsValid) // don't do this if we're zooming or if we're done
	{
		shouldDraw = true; // DRAW
	}
	if (update) // If the user presses the update button, don't do our zooming algorithm
	{
		lastTransform2 = lastTransform;
		lastTransform = currentTransform;
	}

	float avgThreadProgress = 0;
	for (int i = 0; i < NUM_FRACTAL_DRAW_THREADS; i++)
	{
		avgThreadProgress += threadProgress[i];
	}
	avgThreadProgress /= pixelBufferHeight * pixelBufferWidth;
	//if (renderedThisFrame == true) avgThreadProgress = 0;
	drawingProgress = avgThreadProgress;

	return shouldDraw;
}