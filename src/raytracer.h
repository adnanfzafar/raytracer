#pragma once

#include <iostream>
#include "math.h"
#include "primitives.h"
#include "world.h"
#include "camera.h"

class RayTracer {
protected:
	World* world;
	
public:

	RayTracer() 
	{
		world = new World();
	}

	~RayTracer() {
		delete world;
	}

	World* getWorld() { return world; }

	
	int getCornerAndDeltaRaysForCasting(const int window_width, const int window_height, const float fov, Vector4f* cornerRay, Vector4f* deltaW, Vector4f* deltaH) {
		if ((window_width <= 0) || (window_height <= 0) || !cornerRay || !deltaW || !deltaW) {
			return 1;
		}

		// make a vector pointing at the corner of the projection plane
		cornerRay->set(0, sin(fov * window_height * CONST_PI / (360.0f * window_width)));
		cornerRay->set(1, sin(fov * CONST_PI / 360.0f));
		cornerRay->set(2, cornerRay->get(1) / tan(fov * CONST_PI / 360.0f));
		cornerRay->set(3, 1);

		// make a vector that strides across the projection plane in u
		deltaW->set(0, -2 * (cornerRay->get(1)) / window_width, 0, 1);
		
		// make a vector that strides across the projection plant in v
		deltaH->set(-2 * (cornerRay->get(0)) / window_height, 0, 0, 1);

		return 0;
	}



	int rayTraceDepthSceneToPixelBuffer(uint32_t** pixel_buffer, float** depth_buffer, const int window_width, const int window_height, Camera *camera) {


		if (!pixel_buffer || !depth_buffer || !world || world->isEmpty() || (window_width <= 0) || (window_height <= 0) || !camera) {
			return 1;
		}

		// initialize the pixel buffer to all empty
		memset(*pixel_buffer, 0, sizeof(uint32_t) * window_width * window_height);

		// initialize the depth buffer to large positive values
		memset(*depth_buffer, 0, sizeof(float) * window_width * window_height);


		Vector4f ray_corner, delta_down, delta_w, delta_h;
		Vector4f origin;
		
		float depth, max_depth, min_depth;
		min_depth = 99999;
		max_depth = 0;

		if (camera->getOrigin(&origin))
			return 1;

		if (getCornerAndDeltaRaysForCasting(window_width, window_height, camera->getFov(), &ray_corner, &delta_w, &delta_h))
			return 1;

		// for all pixels in the window,
		for (int j = 0; j < window_height; j++)
		{
			// determine the vertical component of the initial ray across the projection plane
			if (delta_down.set(delta_h) || delta_down.multiply(j * 1.0f))
				break;
			
#pragma omp parallel for
			for (int i = 0; i < window_width; i++)
			{
				Vector4f ray;
				Vector4f point;
				Vector4f eyeToPoint;
				Vector4f delta_right;

				// determine the horizontal component of the initial ray across the projection plane
				if(delta_right.set(delta_w) || delta_right.multiply(i*1.0f))
					continue;

				// add the corner to the accumulated right and down endpoints across the projection plane to determine the point p on the projection plane and then the ray
				if(ray.set(delta_down) || ray.add(&delta_right) || ray.add(&ray_corner))
					continue;
				ray.normalize3f();

				// TODO: transform the ray by the camera orientation, but check why it is incorrect
				if(ray.preMultiply(camera->getViewMatrix()->get()))
					continue;
				
				//std::cout << "ray(" << i << "," << j << ") = <" << ray[0] << "," << ray[1] << "," << ray[2] << ">" << std::endl;

				// for all primitives in the scene, 
				for (auto primitive : *(world->getPrimitives()))
				{

					// get intersection point for ray and primitive
					//if (0 == primitive->intersect(origin, &ray_rotated, &point))
					if (0 == primitive->intersect(&origin, &ray, &point))
					{

						// if intersection, get distance to point from camera and use as intensity
						eyeToPoint.set(point);
						eyeToPoint.subtract(&origin);
						depth = eyeToPoint.magnitude();

						// if depth <= depth_buffer[x,y] then color_buffer[x,y] = intensity
						if (depth < (*depth_buffer)[j * window_width + i] || (*depth_buffer)[j * window_width + i] == 0)
						{
							// store the depth and later illuminate the image based on depth (temporary)
							(*depth_buffer)[j * window_width + i] = depth;

						}

					}

				}

			}

		}

		// traverse depth buffer and determine min and max depth (temporary)
		for (int j = 0; j < window_height; j++)
		{
			for (int i = 0; i < window_width; i++)
			{
				if ((*depth_buffer)[j * window_height + i] != 0)
				{
					// store the depth extends to scale the pixel colors based on % depth
					if ((*depth_buffer)[j * window_height + i] < min_depth)
						min_depth = (*depth_buffer)[j * window_height + i];
					if ((*depth_buffer)[j * window_height + i] > max_depth)
						max_depth = (*depth_buffer)[j * window_height + i];
				}
			}
		}

		// traverse depth buffer and determine % depth values to assign as pixel values (temporary)
		for (int j = 0; j < window_height; j++)
		{
			for (int i = 0; i < window_width; i++)
			{
				if ((*depth_buffer)[j * window_height + i] != 0)
				{
					depth = (*depth_buffer)[j * window_height + i];
					(*pixel_buffer)[j * window_height + i] = ((int)((max_depth - depth) / (max_depth - min_depth) * 255)) << 8 | ((int)((max_depth - depth) / (max_depth - min_depth) * 255)) << 16 | ((int)((max_depth - depth) / (max_depth - min_depth) * 255)) << 24;
				}
			}
		}

		//std::cout << "frame" << std::endl;


		return 0;
	}
};