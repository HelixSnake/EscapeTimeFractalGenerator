#include "FractalDrawer.h"
#include "ComplexFractal.h"

std::mutex mtx;

FractalDrawer::FractalDrawer(int width, int height, GLFWwindow* window) 
{
	this->pixelBufferHeight = height;
	this->pixelBufferWidth = width;
	this->window = window;
	pixelBuffer = new float[width * height * 3];
	glGenTextures(1, &fractalTexture);
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

bool FractalDrawer::DrawFractal(float* pixelBuffer, int pixelBufferWidth, int pixelBufferHeight, const float* rampColors, int rampColorsWidth, glm::vec3 transform, std::atomic_bool &halt)
{
	std::lock_guard<std::mutex> lock1{ mtx };
	std::cout << "drawing fractal" << std::endl;
	ComplexFractal fractal = ComplexFractal(40);
	fractal.SetStartingFunction([](ComplexFloat input) {return input; });
	fractal.SetFunction([](ComplexFloat input, ComplexFloat previousValue) {return previousValue * previousValue + ComplexFloat(input.imaginary, input.real); });

	
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
			float value = fractal.CalculateEscapeTime(x, y);
			if (value == 0)
			{
				DrawPixel(pixelBuffer, pixelBufferWidth, pixelBufferHeight, j, i, 0, 0, 0);
			}
			else
			{
				float newValue = pow(value, 0.4);
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
	if (!drawFractalThread.valid() && fractalThreadNeedsRun)
	{
		drawFractalThread = std::async(std::launch::async, &DrawFractal, pixelBuffer, pixelBufferWidth, pixelBufferHeight, rampColors, ramTexWidth, glm::vec3(0.5, 0.5, 2), std::ref(haltDrawingThread));
		fractalThreadNeedsRun = false;
	}
	//DrawFractal(currentPixelBuffer, pixelBufferWidth, pixelBufferHeight, rampColors, ramTexWidth, glm::vec3(0.5, 0.5, 2));
	
	glBindTexture(GL_TEXTURE_2D, fractalTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	std::future_status drawingStatus = std::future_status::timeout;
	if (drawFractalThread.valid())
	{
		drawingStatus = drawFractalThread.wait_for(std::chrono::seconds(0));
		if (drawingStatus == std::future_status::ready && drawFractalThread.get() == true)
		{
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