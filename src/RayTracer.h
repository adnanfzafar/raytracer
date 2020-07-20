#pragma once

#include <iostream>
#include <math.h>

#include "Vector4f.h"
#include "Primitive.h"
#include "Sphere.h"
#include "World.h"
#include "Camera.h"

class RayTracer {
protected:
	World* world;
	
public:

	RayTracer();
	~RayTracer();

	World* getWorld();
	
	int getCornerAndDeltaRaysForCasting(const int window_width, const int window_height, const float fov, Vector4f* cornerRay, Vector4f* deltaW, Vector4f* deltaH);
	
	int rayTraceDepthSceneToPixelBuffer(uint32_t** pixel_buffer, float** depth_buffer, const int window_width, const int window_height, Camera* camera);
	int rayTraceSceneToPixelBuffer(uint32_t** pixel_buffer, float** depth_buffer, const int window_width, const int window_height, Camera* camera);
};