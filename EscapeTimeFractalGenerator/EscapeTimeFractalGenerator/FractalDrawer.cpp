#include "FractalDrawer.h"
#include "ComplexFractal.h"
#include <math.h>
#include <glm/vec2.hpp>
#include <glm/common.hpp>

std::mutex mtx;
const glm::vec3 STARTING_TRANSFORM = glm::vec3(0.5, 0.5, 3);
//const float VALUE_POWER = 0.4;
//const float VALUE_MULT = 0.01;
//const float VALUE_PERIOD = 100;
//const float LENGTH_LIMIT = 100;

FractalDrawer::FractalDrawer(int width, int height, int numExtraValues)
{
	drawFractalThreads = new std::future<bool>[NUM_FRACTAL_DRAW_THREADS];
	threadProgress = new std::atomic<int>[NUM_FRACTAL_DRAW_THREADS];
	Mutexes = new std::mutex[NUM_FRACTAL_DRAW_THREADS];
	drawingStatus = new std::future_status[NUM_FRACTAL_DRAW_THREADS];
	startExecutors = new FractalCommandListExecutor * [NUM_FRACTAL_DRAW_THREADS];
	recursiveExecutors = new FractalCommandListExecutor * [NUM_FRACTAL_DRAW_THREADS];
	for (int i = 0; i < NUM_FRACTAL_DRAW_THREADS; i++)
	{
		startExecutors[i] = nullptr;
		recursiveExecutors[i] = nullptr;
	}

	this->pixelBufferHeight = height;
	this->pixelBufferWidth = width;
	pixelBuffer = new std::atomic<CF_Float>[width * height];
	currentFractal = FractalDictionary::FractalType::Mandelbrot;
	extraValues = new ComplexFloat[numExtraValues];
	this->numExtraValues = numExtraValues;

}

FractalDrawer::~FractalDrawer()
{
	haltDrawingThread = true;
	LockAllMutexes();
	delete[] pixelBuffer;
	delete[] extraValues;
	UnlockAllMutexes();
	delete[] drawFractalThreads;
	delete[] drawingStatus;
	delete[] threadProgress;
	delete[] Mutexes;
	for (int i = 0; i < NUM_FRACTAL_DRAW_THREADS; i++)
	{
		if (startExecutors[i] != nullptr)
			delete startExecutors[i];
		if (recursiveExecutors[i] != nullptr)
			delete recursiveExecutors[i];
	}
	delete[] startExecutors;
	delete[] recursiveExecutors;
}


void FractalDrawer::LockAllMutexes(bool haltDrawing)
{
	if (haltDrawing) haltDrawingThread = true;
	//mtx.lock();
	for (int i = 0; i < NUM_FRACTAL_DRAW_THREADS; i++)
	{
		Mutexes[i].lock();
	}
}
void FractalDrawer::UnlockAllMutexes()
{
	//mtx.unlock();
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

bool FractalDrawer::DrawFractalChunk(int index, CF_Float tfx, CF_Float tfy, CF_Float tfscale, ComplexFloat* extraValues, CF_Float power)
{
	std::lock_guard<std::mutex> lock1{ Mutexes[index] };
	ComplexFractal fractal = ComplexFractal(iterations, minDeviation, deviationCycles, debugDeviations);
	fractal.lengthLimit = lengthLimit;
	FractalDictionary::FractalTypeInfo typeInfo = FractalDictionary::GetInfo(currentFractal);
	if (typeInfo.recursiveFunction != nullptr && typeInfo.startingValueFunction != nullptr)
	{
		fractal.SetFunction(typeInfo.recursiveFunction);
		fractal.SetStartingFunction(typeInfo.startingValueFunction);
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
			CF_Float value = fractal.CalculateEscapeTime(x, y, extraValues, power);
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

bool FractalDrawer::DrawFractalChunkFromCommands(int index, CF_Float tfx, CF_Float tfy, CF_Float tfscale, CF_Float power)
{
	std::lock_guard<std::mutex> lock1{ Mutexes[index] };
	ComplexFractal fractal = ComplexFractal(iterations, minDeviation, deviationCycles, debugDeviations);
	fractal.lengthLimit = lengthLimit;
	FractalDictionary::FractalTypeInfo typeInfo = FractalDictionary::GetInfo(currentFractal);
	if (typeInfo.recursiveFunction != nullptr && typeInfo.startingValueFunction != nullptr)
	{
		fractal.SetFunction(typeInfo.recursiveFunction);
		fractal.SetStartingFunction(typeInfo.startingValueFunction);
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
			startExecutors[index]->SetConstantComplexFloat(0, ComplexFloat(x, y));
			recursiveExecutors[index]->SetConstantComplexFloat(0, ComplexFloat(x, y));
			CF_Float value = fractal.CalculateEscapeTime(*startExecutors[index], *recursiveExecutors[index], power);
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

void FractalDrawer::SetFractalType(FractalDictionary::FractalType fractal)
{
	LockAllMutexes();
	this->currentFractal = fractal;
	UnlockAllMutexes();
}

void FractalDrawer::InstantiateExecutors(FractalCommandList startingFunction, FractalCommandList recursiveFunction, FractalCommandDelegates* delegates)
{
	LockAllMutexes();
	for (int i = 0; i < NUM_FRACTAL_DRAW_THREADS; i++)
	{
		if (startExecutors[i] != nullptr) delete startExecutors[i];
		if (recursiveExecutors[i] != nullptr) delete recursiveExecutors[i];
		startExecutors[i] = new FractalCommandListExecutor(startingFunction, delegates);
		recursiveExecutors[i] = new FractalCommandListExecutor(recursiveFunction, delegates);
	}
	UnlockAllMutexes();
}
bool FractalDrawer::ExecutorsAreInstantiated()
{
	for (int i = 0; i < NUM_FRACTAL_DRAW_THREADS; i++)
	{
		if (startExecutors[i] == nullptr) return false;
		if (recursiveExecutors[i] == nullptr) return false;
	}
	return true;
}


void FractalDrawer::SendConstsToExecutors(std::vector<CF_Float> startFloats, std::vector<CF_Float> recrFloats, std::vector<ComplexFloat> startCFloats, std::vector<ComplexFloat> recrCFloats)
{
	LockAllMutexes();
	for (int i = 0; i < NUM_FRACTAL_DRAW_THREADS; i++)
	{
		if (startExecutors[i] == nullptr || recursiveExecutors[i] == nullptr) return;
		for (int j = 0; j < startFloats.size(); j++)
			startExecutors[i]->SetConstantFloat(j, startFloats[j]);
		for (int j = 0; j < recrFloats.size(); j++)
			recursiveExecutors[i]->SetConstantFloat(j, recrFloats[j]);
		for (int j = 0; j < startCFloats.size(); j++)
			startExecutors[i]->SetConstantComplexFloat(j+1, startCFloats[j]);
		for (int j = 0; j < recrCFloats.size(); j++)
			recursiveExecutors[i]->SetConstantComplexFloat(j+1, recrCFloats[j]);
	}
	UnlockAllMutexes();
}

FractalDictionary::FractalType FractalDrawer::GetFractalType()
{
	return currentFractal;
}

int FractalDrawer::GetMipLevel()
{
	return (int)glm::max(log2(upScale), 0.0f);
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

bool FractalDrawer::Draw(bool update, ZoomTransform transform, ComplexFloat* extraValues, CF_Float power)
{
	bool areWeDone = false; // Value to return from function when we're done
	//Draw Fractal
	bool anyThreadsValid = false;
	for (int i = 0; i < NUM_FRACTAL_DRAW_THREADS; i++)
	{
		//any threads are valid
		anyThreadsValid = anyThreadsValid || drawFractalThreads[i].valid();
	}
	if (!anyThreadsValid && update)
	{
		LockAllMutexes();
		for (int i = 0; i < numExtraValues; i++)
		{
			this->extraValues[i] = extraValues[i];
		}
		UnlockAllMutexes();
		//make sure drawing threads are set to not halt
		haltDrawingThread = false;
		for (int i = 0; i < NUM_FRACTAL_DRAW_THREADS; i++)
		{
			int chunksize = pixelBufferHeight / 16;
			int ystart = i * chunksize;
			int yend = i == 15 ? pixelBufferHeight : (i + 1) * chunksize;
			threadProgress[i] = 0;
			if (!useCustomFunction)
			{
				drawFractalThreads[i] = std::async(std::launch::async, &FractalDrawer::DrawFractalChunk, this, i,
					transform.x, transform.y, transform.scale, this->extraValues, power);
			}
			else
			{
				if (startExecutors[i] != nullptr && recursiveExecutors[i] != nullptr)
				{
					drawFractalThreads[i] = std::async(std::launch::async, &FractalDrawer::DrawFractalChunkFromCommands, this, i,
						transform.x, transform.y, transform.scale, power);
				}
			}
		}
		renderedZoom = transform;
		isBusy = true;
	}
	bool allThreadsValid = true;
	bool allThreadsReady = true;
	bool renderedThisFrame = false;
	shouldRestartInterpreter = false;
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
			areWeDone = !haltDrawingThread; // draw if drawing thread was not halted
			shouldRestartInterpreter = !haltDrawingThread; // if the interpreter is drawing, and we just finished, make it start over, unless we're zooming
			renderedThisFrame = true;
			UnlockAllMutexes();
			isBusy = false; // we're done!
		}
	}

	float avgThreadProgress = 0;
	for (int i = 0; i < NUM_FRACTAL_DRAW_THREADS; i++)
	{
		avgThreadProgress += threadProgress[i];
	}
	avgThreadProgress /= pixelBufferHeight * pixelBufferWidth;
	//if (renderedThisFrame == true) avgThreadProgress = 0;
	drawingProgress = avgThreadProgress;

	return areWeDone;
}