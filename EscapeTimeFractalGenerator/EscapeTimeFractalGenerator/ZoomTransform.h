#pragma once
#include "ComplexFloat.h"
struct ZoomTransform
{
	CF_Float x = 0;
	CF_Float y = 0;
	CF_Float scale = 1;
	ZoomTransform() {}
	ZoomTransform(CF_Float x, CF_Float y, CF_Float scale)
	{
		this->x = x; this->y = y; this->scale = scale;
	}
	void ZoomInOrOut(CF_Float centerX, CF_Float centerY, CF_Float amount, float aspectRatio)
	{
		//convert x y point to "World space" by replicating transform performed on pixels
		CF_Float newX = (centerX * aspectRatio - x) * scale;
		CF_Float newY = (centerY - y) * scale;
		scale *= amount;
		x -= (newX / scale) * (1 - amount);
		y -= (newY / scale) * (1 - amount);
	}
	ComplexFloat ScreenToWorldPos(CF_Float screenX, CF_Float screenY, float aspectRatio)
	{
		CF_Float newX = (screenX * aspectRatio - x) * scale;
		CF_Float newY = (screenY - y) * scale;
		return ComplexFloat(newX, newY);
	}
};