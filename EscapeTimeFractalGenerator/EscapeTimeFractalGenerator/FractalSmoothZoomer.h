#pragma once
#include <glm/vec4.hpp>
#include <glm/common.hpp>
#include "ComplexFloat.h"
#include "ZoomTransform.h"
class FractalSmoothZoomer
{
public:
	FractalSmoothZoomer();
	glm::vec4 GetBounds(float progress, float aspectRatio);
	void SetupZoom(ZoomTransform start, ZoomTransform end);
	void StartZoom();
	void EndZoom();
	bool IsZooming() { return isZooming; }
	bool IsZoomReady() { return isZoomReady; }
protected:
	ZoomTransform transformStart;
	ZoomTransform transformEnd;
	bool isZooming = false;
	bool isZoomReady = false;
};

