#pragma once
//GLEW
#define GLEW_STATIC
#include <GL/glew.h>
//GLFW
#include <GLFW/glfw3.h>

class QuadDrawer
{
public:
	QuadDrawer();
	~QuadDrawer();
	bool DrawBuffer(GLFWwindow* window, const float* colors, GLenum format, int width, int height, int x1, int y1, int x2, int y2, bool generateTexture); //returns whether or not the drawing succeeded
protected:
	GLuint drawingTexture = 0;
	GLuint vertexShader = 0;
	GLuint fragmentShader = 0;
	GLuint shaderProgram = 0;
};

