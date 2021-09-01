#include "FractalSmoothZoomer.h"

FractalSmoothZoomer::FractalSmoothZoomer()
{
	transformStart.x = 0;
	transformStart.y = 0;
	transformStart.scale = 1;
	transformEnd = transformStart;
	cachedTransformEnd = transformStart;
	cachedTransformStart = transformStart;
}

glm::vec4 FractalSmoothZoomer::GetBounds(float progress, float aspectRatio)
{
	if (!isZooming) return glm::vec4(0, 0, 1, 1);
	CF_Float scaleDiff = transformStart.scale / transformEnd.scale;
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
		rendRectNewX1 = rendRectX1 * scaleDiff + (transformEnd.x - transformStart.x + (transformStart.x * (1 - scaleDiff))) * aspectRatio;
		rendRectNewY1 = rendRectY1 * scaleDiff + transformEnd.y - transformStart.y + (transformStart.y * (1 - scaleDiff));
		rendRectNewX2 = rendRectX2 * scaleDiff + (transformEnd.x - transformStart.x + (transformStart.x * (1 - scaleDiff))) * aspectRatio;
		rendRectNewY2 = rendRectY2 * scaleDiff + transformEnd.y - transformStart.y + (transformStart.y * (1 - scaleDiff));
	}
	rendRectX1 = glm::mix(rendRectX1, rendRectNewX1, progress);
	rendRectX2 = glm::mix(rendRectX2, rendRectNewX2, progress);
	rendRectY1 = glm::mix(rendRectY1, rendRectNewY1, progress);
	rendRectY2 = glm::mix(rendRectY2, rendRectNewY2, progress);
	return glm::vec4(rendRectX1, rendRectY1, rendRectX2, rendRectY2);
}


void FractalSmoothZoomer::SetupZoom(ZoomTransform start, ZoomTransform end)
{
	cachedTransformStart = start;
	cachedTransformEnd = end;
	isZoomReady = true;
}

void FractalSmoothZoomer::StartZoom()
{
	transformStart = cachedTransformStart;
	transformEnd = cachedTransformEnd;
	isZooming = true;
	isZoomReady = false;
}

void FractalSmoothZoomer::EndZoom()
{
	isZooming = false;
}