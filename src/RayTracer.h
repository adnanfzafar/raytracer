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
	std::vector<Vector4f*>* eyeRays;
	bool eyeRaysValid;
	
public:

	RayTracer();
	~RayTracer();

	World* getWorld() { return world; }
	bool getEyeRaysValid() { return eyeRaysValid; }
	std::vector<Vector4f*>* getEyeRays() { return eyeRays; }

	int getCornerAndDeltaRaysForCasting(const int window_width, const int window_height, const float fov, Vector4f* cornerRay, Vector4f* deltaW, Vector4f* deltaH);
	int calculateEyeRays(const int window_width, const int window_height, Camera *camera);
	
	
	int rayTraceDepthSceneToPixelBuffer(uint32_t** pixel_buffer, float** depth_buffer, const int window_width, const int window_height, Camera* camera);
	int rayTraceSceneToPixelBuffer(uint32_t** pixel_buffer, float** depth_buffer, const int window_width, const int window_height, Camera* camera);
	int rayTraceSceneToPixelBufferFast(uint32_t** pixel_buffer, float** depth_buffer, const int window_width, const int window_height, Camera* camera);
};