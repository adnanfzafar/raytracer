#include "RayTracer.h"

RayTracer::RayTracer()
{
	world = new World();
	eyeRays = new std::vector<Vector4f*>();
	eyeRaysValid = false;
}

RayTracer::~RayTracer() {
	delete world;

	eyeRays->clear();
	delete eyeRays;

	eyeRaysValid = false;
}

int RayTracer::getCornerAndDeltaRaysForCasting(const int window_width, const int window_height, const float fov, Vector4f* cornerRay, Vector4f* deltaW, Vector4f* deltaH)
{
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


int RayTracer::calculateEyeRays(const int window_width, const int window_height, Camera *camera)
{
	
	if (!eyeRays || (window_width <= 0) || (window_height <= 0) || !camera || (camera->getFov() <= 0))
		return 1;

	eyeRays->clear();
	eyeRaysValid = false;

	Vector4f cornerRay, deltaW, deltaH, deltaDown, deltaRight;

	if (getCornerAndDeltaRaysForCasting(window_width, window_height, camera->getFov(), &cornerRay, &deltaW, &deltaH))
		return 1;

	// for all pixels in the window,
	for (int j = 0; j < window_height; j++)
	{
		// determine the vertical component of the initial ray across the projection plane
		if (deltaDown.set(deltaH) || deltaDown.multiply(j * 1.0f))
			break;

//#pragma omp parallel for
		for (int i = 0; i < window_width; i++)
		{
			Vector4f ray;
			//Vector4f point;
			//Vector4f eyeToPoint;
			Vector4f delta_right;
			hit_record_s hit_record;

			// determine the horizontal component of the initial ray across the projection plane
			if (delta_right.set(deltaW) || delta_right.multiply(i * 1.0f))
				return 1;

			// add the corner to the accumulated right and down endpoints across the projection plane to determine the point p on the projection plane and then the ray
			if (ray.set(deltaDown) || ray.add(&delta_right) || ray.add(&cornerRay))
				return 1;
			ray.normalize3f();

			// transform the ray by the camera orientation
			if (ray.preMultiply(camera->getViewMatrix()->get()))
				return 1;

			eyeRays->push_back(new Vector4f(ray));
		}
	}

	eyeRaysValid = true;

	return 0;
}

int RayTracer::rayTraceDepthSceneToPixelBuffer(uint32_t** pixel_buffer, float** depth_buffer, const int window_width, const int window_height, Camera* camera) {


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
			//Vector4f point;
			//Vector4f eyeToPoint;
			Vector4f delta_right;
			hit_record_s hit_record;

			// determine the horizontal component of the initial ray across the projection plane
			if (delta_right.set(delta_w) || delta_right.multiply(i * 1.0f))
				continue;

			// add the corner to the accumulated right and down endpoints across the projection plane to determine the point p on the projection plane and then the ray
			if (ray.set(delta_down) || ray.add(&delta_right) || ray.add(&ray_corner))
				continue;
			ray.normalize3f();

			// TODO: transform the ray by the camera orientation, but check why it is incorrect
			if (ray.preMultiply(camera->getViewMatrix()->get()))
				continue;

			//std::cout << "ray(" << i << "," << j << ") = <" << ray[0] << "," << ray[1] << "," << ray[2] << ">" << std::endl;

			// for all primitives in the scene, 
			for (auto primitive : *(world->getPrimitives()))
			{

				// get intersection point for ray and primitive
				//if (0 == primitive->intersect(origin, &ray_rotated, &point))
				if (0 == primitive->intersect(&origin, &ray, &hit_record))
				{

					// if intersection, get distance to point from camera and use as intensity
					/*
					eyeToPoint.set(point);
					eyeToPoint.subtract(&origin);
					depth = eyeToPoint.magnitude();
					*/
					hit_record.point.subtract(&origin);
					depth = hit_record.point.magnitude();

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
			if ((*depth_buffer)[j * window_width + i] != 0)
			{
				depth = (*depth_buffer)[j * window_width + i];
				(*pixel_buffer)[j * window_width + i] = ((int)((max_depth - depth) / (max_depth - min_depth) * 255)) << 8 | ((int)((max_depth - depth) / (max_depth - min_depth) * 255)) << 16 | ((int)((max_depth - depth) / (max_depth - min_depth) * 255)) << 24;
			}
		}
	}

	//std::cout << "frame" << std::endl;


	return 0;
}

#define max(a,b) a>b ? a : b;

int RayTracer::rayTraceSceneToPixelBuffer(uint32_t** pixel_buffer, float** depth_buffer, const int window_width, const int window_height, Camera* camera) {


	if (!pixel_buffer || !depth_buffer || !world || world->isEmpty() || (window_width <= 0) || (window_height <= 0) || !camera) {
		return 1;
	}

	// initialize the pixel buffer to all empty
	memset(*pixel_buffer, 0, sizeof(uint32_t) * window_width * window_height);

	// initialize the depth buffer to large positive values
	memset(*depth_buffer, 0, sizeof(float) * window_width * window_height);

	// calculate eye rays if they havent been determined already
	if (eyeRaysValid == false)
	{
		if (calculateEyeRays(window_width, window_height, camera))
		{
			return 1;
		}
	}
	Vector4f origin;

	float depth, max_depth, min_depth;
	min_depth = 99999;
	max_depth = 0;

	if (camera->getOrigin(&origin))
		return 1;

	
	// for all pixels in the window,
	for (int j = 0; j < window_height; j++)
	{

#pragma omp parallel for
		for (int i = 0; i < window_width; i++)
		{
			Vector4f eyeToPoint;
			Vector4f delta_right;
			hit_record_s hit_record;

			
			// for all primitives in the scene, 
			for (auto primitive : *(world->getPrimitives()))
			{

				// get intersection point for ray and primitive
				//if (0 == primitive->intersect(origin, &ray_rotated, &point))
				//if (0 == primitive->intersect(&origin, &ray, &hit_record))
				if (0 == primitive->intersect(&origin, eyeRays->at(j*window_width + i), &hit_record))
				{

					// if intersection, get the depth of the ray / scene intersection
					eyeToPoint.set(&(hit_record.point));
					eyeToPoint.subtract(&origin);
					depth = eyeToPoint.magnitude();


					// if depth <= depth_buffer[x,y] then color_buffer[x,y] = intensity
					if (depth < (*depth_buffer)[j * window_width + i] || (*depth_buffer)[j * window_width + i] == 0)
					{
						// store the depth and later illuminate the image based on material color directly (temporary)
						(*depth_buffer)[j * window_width + i] = depth;

						// Phong illumination model
						// TODO: add a framework for selecting a different lighting model
						// if hit, add contribution from all lights
						for (auto light : *(world->getLights()))
						{
							float ndotl, rdotv, specular_term, diffuse_term;
							Vector4f Lhat, Rhat, Vhat;

							// compute L hat unit vector in direciton from surface point to light
							if (Lhat.set(light->getOrigin()) || Lhat.subtract(&(hit_record.point)))
								continue;
							Lhat.normalize3f();

							// TODO: ensure that L does not intersect with another object to allow for shadows

							// compute V hat unit vector in direction from surface point to viewpoint
							if (Vhat.set(origin) || Vhat.subtract(&(hit_record.point)))
								continue;
							Vhat.normalize3f();

							// compute R hat unit reflection vector at surface point
							if (Rhat.set(hit_record.normal) || Rhat.multiply(2 * hit_record.normal.innerProduct3f(&Lhat)) || Rhat.subtract(&Lhat))
								continue;

							//I[channel] = ambient channel + f_att*Ip[color]*(kd*Od[color]*(Nhat dot Lhat) + ks*(Rhat dot Vhat) ^ n)
							ndotl = max(hit_record.normal.innerProduct3f(&Lhat), 0);
							rdotv = max(Rhat.innerProduct3f(&Vhat), 0);
							specular_term = pow(rdotv, hit_record.material->n) * rdotv * hit_record.material->ks;
							diffuse_term = hit_record.material->kd * ndotl;

							// TODO: optimize accumulation and channels
							(*pixel_buffer)[j * window_width + i] += (int)(light->getColor()->get()[0] * (hit_record.diffuseColor.get()[0] * diffuse_term + specular_term) * 255) << 8;
							(*pixel_buffer)[j * window_width + i] += (int)(light->getColor()->get()[1] * (hit_record.diffuseColor.get()[1] * diffuse_term + specular_term) * 255) << 16;
							(*pixel_buffer)[j * window_width + i] += (int)(light->getColor()->get()[2] * (hit_record.diffuseColor.get()[2] * diffuse_term + specular_term) * 255) << 24;
						}

					}

				}

			}

		}

	}



	//std::cout << "frame" << std::endl;


	return 0;
}
