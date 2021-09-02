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
	void SetupZoom(ZoomTransform newZoomTransform);
	void UpdateTarget(ZoomTransform newZoomTransform);
	void StartZoom();
	void EndZoom();
	void SyncTransforms(ZoomTransform newZoomTransform);
	bool IsZooming() { return isZooming; }
	bool IsZoomReady() { return isZoomReady; }
	void DebugGetTransforms(ZoomTransform& start, ZoomTransform& end, ZoomTransform& cstart, ZoomTransform& cend)
	{
		start = transformStart;
		end = transformEnd;
		cstart = cachedTransformStart;
		cend = cachedTransformEnd;
	}
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
	float startingInterpreterProgress = 0;
	float drawerFinishProgressTarget = 0;
	steady_clock::time_point zoomStartTime;
};

