#pragma once
#include <glm/vec4.hpp>
#include <glm/common.hpp>
#include "ComplexFloat.h"
#include "ZoomTransform.h"
class FractalSmoothZoomer
{
public:
	FractalSmoothZoomer();
	glm::vec4 GetBoundMults(float aspectRatio);
	void SetupZoom(ZoomTransform start, ZoomTransform end);
	void StartZoom();
	void EndZoom();
	bool IsZooming() { return isZooming; }
	bool IsZoomReady() { return isZoomReady; }
	void RunProgressLogic(float DrawerProgress, float InterpreterProgress);
protected:
	ZoomTransform cachedTransformStart;
	ZoomTransform cachedTransformEnd;
	ZoomTransform transformStart;
	ZoomTransform transformEnd;
	bool isZooming = false;
	bool isZoomReady = false;
	bool justStartedZooming = false;
	float computedProgress = 0;
	float startingDrawerProgress = 0;
};

