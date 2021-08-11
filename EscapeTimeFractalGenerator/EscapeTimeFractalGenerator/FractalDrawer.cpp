#include "FractalDrawer.h"
#include "ComplexFractal.h"


FractalDrawer::FractalDrawer(int width, int height, GLFWwindow* window) 
{
	this->pixelBufferHeight = height;
	this->pixelBufferWidth = width;
	this->window = window;
	pixelBuffer1 = new float[width * height * 3];
	pixelBuffer2 = new float[width * height * 3];
	currentPixelBuffer = pixelBuffer1;
}

FractalDrawer::~FractalDrawer()
{
		delete[] pixelBuffer1;
		delete[] pixelBuffer2;
}


void FractalDrawer::SetRampTexture(GLuint textureID)
{
	rampTexture = textureID;
}

void FractalDrawer::DrawPixel(float* pixelBuffer, int pixelBufferWidth, int pixelBufferHeight, int x, int y, float r, float g, float b)
{
	if (x < 0 || x >= pixelBufferWidth || y < 0 || y >= pixelBufferHeight) return;
	int startIndex = 3 * (y * pixelBufferWidth + x);
	pixelBuffer[startIndex] = r;
	pixelBuffer[startIndex + 1] = g;
	pixelBuffer[startIndex + 2] = b;
}

void FractalDrawer::DrawFractal(float* pixelBuffer, int pixelBufferWidth, int pixelBufferHeight, GLuint rampTexture, glm::vec3 transform)
{
	ComplexFractal fractal = ComplexFractal(40);
	fractal.SetStartingFunction([](ComplexFloat input) {return input; });
	fractal.SetFunction([](ComplexFloat input, ComplexFloat previousValue) {return previousValue * previousValue + ComplexFloat(input.imaginary, input.real); });

	GLfloat* rampColor = nullptr;
	GLint ramTexWidth = 0;
	if (rampTexture != 0)
	{
		glBindTexture(GL_TEXTURE_2D, rampTexture);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &ramTexWidth);
		rampColor = new GLfloat[ramTexWidth * 3];
		glGetnTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, ramTexWidth * 3 * sizeof(GLfloat), rampColor);
	}
	for (int i = 0; i < pixelBufferHeight; i++)
	{
		for (int j = 0; j < pixelBufferWidth; j++)
		{
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
				if (rampColor != nullptr)
				{
					int rampIndex = ((int)((float)ramTexWidth * (1 - newValue)));
					if (rampIndex > ramTexWidth - 1) rampIndex = ramTexWidth - 1;
					rampIndex *= 3;
					DrawPixel(pixelBuffer, pixelBufferWidth, pixelBufferHeight, j, i, rampColor[rampIndex], rampColor[rampIndex + 1], rampColor[rampIndex + 2]);
					//DrawPixel(pixelBuffer, pixelBufferWidth, pixelBufferHeight, j, i, value, value, value);
				}
				else
					DrawPixel(pixelBuffer, pixelBufferWidth, pixelBufferHeight, j, i, newValue, newValue, newValue);
			}
		}
	}
	if (rampColor != nullptr)
	{
		delete[] rampColor;
	}
}

bool FractalDrawer::Draw()
{
	//Draw Fractal
	DrawFractal(currentPixelBuffer, pixelBufferWidth, pixelBufferHeight, rampTexture, glm::vec3(0.5, 0.5, 2));

	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pixelBufferWidth, pixelBufferHeight, 0, GL_RGB, GL_FLOAT, currentPixelBuffer);

	GLuint fbo = 0;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, tex, 0);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	int width = 0;
	int height = 0;
	glfwGetWindowSize(window, &width, &height);
	glBlitFramebuffer(0, 0, pixelBufferWidth, pixelBufferHeight, 0, 0, width, height,
		GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glDeleteFramebuffers(1, &fbo);
	glDeleteTextures(1, &tex);
	return true;
}