#include "QuadDrawer.h"
#include <math.h>

QuadDrawer::QuadDrawer()
{
	glGenTextures(1, &drawingTexture);
}
QuadDrawer::~QuadDrawer()
{
	glDeleteTextures(1, &drawingTexture);
}
bool QuadDrawer::DrawBuffer(GLFWwindow* window, const unsigned char* colors, GLenum format, int width, int height, int x1, int y1, int x2, int y2, int miplevel, bool generateTexture)
{
	if (format != GL_RGB && format != GL_RGBA) return false; // currently supported formats
	glBindTexture(GL_TEXTURE_2D, drawingTexture);
	if (generateTexture)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, colors);
		if (miplevel != 0)
		{
			glGenerateTextureMipmap(drawingTexture);
		}
	}

	GLuint fbo = 0;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, drawingTexture, miplevel);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	int windowWidth = 0;
	int windowHeight = 0;
	glfwGetWindowSize(window, &windowWidth, &windowHeight);
	float frameBufferScale = pow(2, miplevel);
	glBlitFramebuffer(0, 0, width / frameBufferScale, height / frameBufferScale, x1, y1, x2, y2,
		GL_COLOR_BUFFER_BIT, GL_LINEAR);
	glDeleteFramebuffers(1, &fbo);
	return true;
}
