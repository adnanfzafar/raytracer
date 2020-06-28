#pragma once

#include <iostream>
#include "math.h"
#include "primitives.h"
#include "world.h"

class RayTracer {
protected:
	World* world;
	vec3_t eye;
	vec3_t up;
	vec3_t right;
	vec3_t origin;
	
	float fov;

	int toggle;

public:
	RayTracer(const vec3_t new_eye, const vec3_t new_up, const vec3_t new_origin, const float new_fov) {
		world = new World();
		//memcpy(&eye, &new_eye, sizeof(vec3_t));
		eye[0] = new_eye[0];
		eye[1] = new_eye[1];
		eye[2] = new_eye[2];
		//memcpy(&up, &new_up, sizeof(vec3_t));
		up[0] = new_up[0];
		up[1] = new_up[1];
		up[2] = new_up[2];
		crossProduct(&eye, &up, &right);
		//memcpy(&origin, &new_origin, sizeof(vec3_t));
		origin[0] = new_origin[0];
		origin[1] = new_origin[1]; 
		origin[2] = new_origin[2];
		fov = new_fov;

		toggle = 0;
	}

	~RayTracer() {
		delete world;
	}

	World* getWorld() { return world; }

	int getCornerAndDeltaRaysForCasting(const int window_width, const int window_height, const float fov, vec3_t* corner_ray, vec3_t *delta_w, vec3_t *delta_h) {
		if ((window_width <= 0) || (window_height <= 0) || !corner_ray || !delta_w || !delta_h) {
			return 1;
		}

		// make a vector pointing at the corner of the projection plane
		(*corner_ray)[0] = sin(fov*window_height * CONST_PI / (360.0f* window_width));
		(*corner_ray)[1] = sin(fov * CONST_PI / 360.0f);
		(*corner_ray)[2] = ((*corner_ray)[1])/tan(fov * CONST_PI / 360.0f);

		// make a vector that strides across the projection plane in u
		(*delta_w)[0] = 0;
		(*delta_w)[1] = -2* ((*corner_ray)[1]) / window_width;
		(*delta_w)[2] = 0;

		// make a vector that strides across the projection plant in v
		(*delta_h)[0] = -2 * ((*corner_ray)[0]) / window_height;
		(*delta_h)[1] = 0;
		(*delta_h)[2] = 0;

		return 0;
	}

	
	int rayTraceDepthSceneToPixelBuffer(uint32_t** pixel_buffer, float** depth_buffer, const int window_width, const int window_height) {
		

		if (!pixel_buffer || !depth_buffer || !world || world->isEmpty() || (window_width <= 0) || (window_height <= 0)) {
			return 1;
		}

		// initialize the pixel buffer to all empty
		memset(*pixel_buffer, 0, sizeof(uint32_t) * window_width * window_height);

		// initialize the depth buffer to large positive values
		memset(*depth_buffer, 0, sizeof(float) * window_width * window_height);

		vec3_t ray;
		vec3_t ray_corner, delta_right, delta_down, delta_w, delta_h;
		vec3_t down;
		memcpy(&down, &up, sizeof(vec3_t));
		scaleVector(&up, -1);
		vec3_t point;
		vec3_t eyeToPoint;
		float depth, max_depth, min_depth;
		min_depth = 99999;
		max_depth = 0;

		if (getCornerAndDeltaRaysForCasting(window_width, window_height, fov, &ray_corner, &delta_w, &delta_h))
			return 1;

		if (vectorClear(&delta_right) || vectorClear(&delta_down))
			return 1;

		// for all pixels in the window,
		for (int j = 0; j < window_height; j++)
		{

			for (int i = 0; i < window_width; i++)
			{
				// add the corner to the accumulated right and down endpoints across the projection plane to determine the point p on the projection plane and then the ray
				if (vectorAdd(&delta_right, &delta_down, &ray) || vectorAdd(&ray, &ray_corner, &ray) || normalize(&ray))// || vectorSubtract(&ray, &origin, &ray))
					return 1;
		
				// TODO: transform the ray by the camera orientation

				//std::cout << "ray(" << i << "," << j << ") = <" << ray[0] << "," << ray[1] << "," << ray[2] << ">" << std::endl;

				// for all primitives in the scene, 
				for (auto primitive : *(world->getPrimitives())) 
				{
					memset(&point, 0, sizeof(vec3_t));
					
					// get intersection point for ray and primitive
					if (0 == primitive->intersect(&origin, &ray, &point)) 
					{
						
						// if intersection, get distance to point from camera and use as intensity
						eyeToPoint[0] = point[0] - origin[0];
						eyeToPoint[1] = point[1] - origin[1];
						eyeToPoint[2] = point[2] - origin[2];
						depth = vectorMagnitude(&eyeToPoint);
						// if depth <= depth_buffer[x,y] then color_buffer[x,y] = intensity
						if (depth < (*depth_buffer)[j * window_width + i] || (*depth_buffer)[j * window_width + i] == 0)
						{
							// store the depth and later illuminate the image based on depth (temporary)
							(*depth_buffer)[j * window_width + i] = depth;
							
						}

						// store the depth extends to scale the pixel colors based on % depth
						if (depth < min_depth)
							min_depth = depth;
						if (depth > max_depth)
							max_depth = depth;

					}
					
				}

				// stride the ray by a delta vector across the width of the projection plane
				if (vectorAdd(&delta_right, &delta_w, &delta_right))
					break;
			}

			// stride the ray by a delta vector across the height of the projection plane
			if (vectorAdd(&delta_down, &delta_h, &delta_down))
				break;

			// clear the right striding vector to start the next scan across the width of the projection plane
			if (vectorClear(&delta_right))
				return 1;
		}

		// traverse depth buffer and determine % depth values to assign as pixel values (temporary)
		for (int j = 0; j < window_height; j++)
		{
			for (int i = 0; i < window_width; i++)
			{
				if ((*depth_buffer)[j * window_height + i] != 0)
				{
					depth = (*depth_buffer)[j * window_height + i];
					(*pixel_buffer)[j * window_height + i] = ((int)((max_depth - depth) / (max_depth- min_depth) * 255)) << 8 | ((int)((max_depth - depth) / (max_depth - min_depth) * 255)) << 16 | ((int)((max_depth - depth) / (max_depth - min_depth) * 255)) << 24;
				}
			}
		}

		//std::cout << "frame" << std::endl;


		return 0;
	}
};