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

void FractalDrawer::SetFractalType(FractalType fractal)
{
	LockAllMutexes();
	this->currentFractal = fractal;
	UnlockAllMutexes();
}
FractalType FractalDrawer::GetFractalType()
{
	return currentFractal;
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
	transformChanged = true;
}
void FractalDrawer::ResetZoom()
{
	haltDrawingThread = true;
	LockAllMutexes();
	currentTransform.x = STARTING_TRANSFORM.x;
	currentTransform.y = STARTING_TRANSFORM.y;
	currentTransform.scale = STARTING_TRANSFORM.z;
	lastDrawnTransform = currentTransform;
	UnlockAllMutexes();
	haltDrawingThread = false;
}

bool FractalDrawer::ShouldStartZoomInterpolation()
{
	return startInterpolateZooming;
}

ZoomTransform FractalDrawer::GetCurrentTransform()
{
	return currentTransform;
}
ZoomTransform FractalDrawer::GetLastDrawnTransform()
{
	return lastDrawnTransform;
}

ComplexFloat FractalDrawer::ScreenToWorldPos(double x, double y)
{
	CF_Float newX = (x * pixelBufferWidth / pixelBufferHeight - currentTransform.x) * currentTransform.scale;
	CF_Float newY = (y - currentTransform.y) * currentTransform.scale;
	return ComplexFloat(newX, newY);
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
	startInterpolateZooming = false; // Should we start the zooming interpolation?
	fractalThreadNeedsRun = fractalThreadNeedsRun || update || transformChanged;
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
		lastDrawnTransform = currentTransform;
		lastTimeDelta = high_resolution_clock::now();
		fractalThreadNeedsRun = false;
		transformChanged = false;
	}
	if (!enableAnimation)
	{
		lastTimeAnim = high_resolution_clock::now();
	}

	steady_clock::time_point currentTimeDelta = high_resolution_clock::now();
	double timeSinceLastRender = duration_cast<nanoseconds>(currentTimeDelta - lastTimeDelta).count() / 1000000000.0;
	disableZoom = false;
/*	if (timeSinceLastRender > 1 && transformChanged) // We don't want the delay on zooming in or out to be more than a second
	{
		haltDrawingThread = true;
		shouldDraw = false;
		startInterpolateZooming = true;
	}*/
	if (transformChanged && timeSinceLastRender > 2)
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
			shouldDraw = !haltDrawingThread; // draw if drawing thread was not halted
			renderedThisFrame = true;
			UnlockAllMutexes();
			if (transformChanged)
			{
				startInterpolateZooming = true;
			}
		}
	}
	//POTENTIAL GLITCHY BEHAVIOR: REMOVE IF THE PROGRAM BREAKS IN ANY WAY
	//pixelBuffer has been moved from float* to std::atomic<float>* so this should be safe now 
	if (liveUpdate && anyThreadsValid && !transformChanged) // don't do this if we're zooming or if we're done
	{
		shouldDraw = true; // draw asyncroneously 
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