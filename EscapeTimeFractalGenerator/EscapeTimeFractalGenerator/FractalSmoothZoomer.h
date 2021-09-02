#pragma once
#include <glm/vec4.hpp>
#include <glm/common.hpp>
#include <math.h>
#include <chrono>
#include "ComplexFloat.h"
#include "ZoomTransform.h"

using namespace std::chrono;

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
	void RunProgressLogic(float DrawerProgress, float InterpreterProgress, float lastInterpreterTime);
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
	float drawerFinishProgressTarget = 0;
	steady_clock::time_point zoomStartTime;
};

