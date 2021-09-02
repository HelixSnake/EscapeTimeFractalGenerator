#include "FractalSmoothZoomer.h"
#include <math.h>

FractalSmoothZoomer::FractalSmoothZoomer()
{
	transformStart.x = 0;
	transformStart.y = 0;
	transformStart.scale = 1;
	transformEnd = transformStart;
	cachedTransformEnd = transformStart;
	cachedTransformStart = transformStart;
}

glm::vec4 FractalSmoothZoomer::GetBoundMults(float aspectRatio)
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
	rendRectX1 = glm::mix(rendRectX1, rendRectNewX1, computedProgress);
	rendRectX2 = glm::mix(rendRectX2, rendRectNewX2, computedProgress);
	rendRectY1 = glm::mix(rendRectY1, rendRectNewY1, computedProgress);
	rendRectY2 = glm::mix(rendRectY2, rendRectNewY2, computedProgress);
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
	justStartedZooming = true;
}

void FractalSmoothZoomer::EndZoom()
{
	justStartedZooming = false; // just in case we run EndZoom before RunProgressLogic runs
	isZooming = false;
}


void FractalSmoothZoomer::RunProgressLogic(float drawerProgress, float interpreterProgress, float lastInterpreterTime)
{
	if (justStartedZooming)
	{
		startingDrawerProgress = drawerProgress; // we're partway into the drawer progress at this point so save this value
		zoomStartTime = high_resolution_clock::now();
		computedProgress = 0;
		drawerFinishProgressTarget = 0;
	}
	justStartedZooming = false;
	float oldComputedProgress = computedProgress;
	if (startingDrawerProgress >= 0.99) // we somehow started with the drawer being finished
	{
		if (drawerProgress < startingDrawerProgress)
		{
			computedProgress = drawerProgress; //just use the interpreter progress in this case, this is the best we can do in this situation
		}
	}
	else
	{
		if (interpreterProgress > 0.99 && drawerProgress >= startingDrawerProgress) 
		{
			//scale our target value we want to be at based on how much time has elapsed so that it will lead smoothly into the interpreter rendering
			steady_clock::time_point currentTime = high_resolution_clock::now();
			float totalDrawTime = duration_cast<nanoseconds>(currentTime - zoomStartTime).count() / 1000000000.0;
			float ratio = totalDrawTime / (totalDrawTime + lastInterpreterTime);
			drawerFinishProgressTarget = startingDrawerProgress + ratio * (1.0f - startingDrawerProgress); // this is the value we want computedProgress to be at when the drawer finishes
			//this is not exactly linear movement but it should be close enough
			computedProgress = (drawerProgress - startingDrawerProgress) / (1.0f - startingDrawerProgress) * drawerFinishProgressTarget;
		}
		else
		{
			// time for the interpreter to take up the remaining time
			computedProgress = drawerFinishProgressTarget + interpreterProgress * (1.0f - drawerFinishProgressTarget); // lerp the rest of the way
		}
	}
	computedProgress = glm::max(computedProgress, oldComputedProgress); // our progress should never be lowered
}