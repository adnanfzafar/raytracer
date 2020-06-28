#pragma once

#include <iostream>
#include "math.h"

class Primitive {
protected:
	vec3_t origin;

public:
	int setOrigin(const vec3_t o) {
		
		origin[0] = o[0];
		origin[1] = o[1];
		origin[2] = o[2];

		return 0;
	}

	vec3_t* getOrigin() { return &origin; }

	virtual int intersect(const vec3_t *ray_origin, const vec3_t *ray, vec3_t *intersection) = 0;
	virtual int getNormal(const vec3_t* point, vec3_t* normal) = 0;
};

class Sphere : public Primitive {
protected:
	float radius;

public:
	int setRadius(float r) {
		if (r <= 0) {
			radius = 10;
			return 1;
		}
		radius = r;
		return 0;
	}

	Sphere(float r, const vec3_t o) {
		setRadius(r);
		setOrigin(o);
	}
	~Sphere() {};

	int getNormal(const vec3_t* point, vec3_t* normal)
	{
		if (!point || !normal)
		{
			return 1;
		}

		*normal[0] = *point[0] - origin[0];
		*normal[1] = *point[1] - origin[1];
		*normal[2] = *point[2] - origin[2];

		if(normalize(normal))
			return 1;
		
		return 0;
	}

	int intersect(const vec3_t* ray_origin, const vec3_t* ray, vec3_t* intersection) {
	
		// Let s = origin - ray_origin
		vec3_t s;
		if (vectorSubtract(&origin, ray_origin, &s))
			return 1;

		// Let projsonv = Project s onto v = (s . v)/|v| * v
		vec3_t projsonv;
		if (vectorCopy(&projsonv, ray) || scaleVector(&projsonv, dotProduct(&s, ray)/vectorMagnitude(ray)))
			return 1;


		// Then let q' = end point of the projection in the global coordinate system: ray_origin + projsonv
		vec3_t q_prime;
		if (vectorAdd(ray_origin, &projsonv, &q_prime))
			return 1;

		// Then, the distance dist from the center of the sphere origin to the end of the global projection q' made by the ray is d = |origin - q'|
		vec3_t d;
		if (vectorSubtract(&origin, &q_prime, &d))
			return 1;
		float dist = vectorMagnitude(&d);

		//std::cout << dist << std::endl;

		// If d < r -> intersect at 2 points
		// else if d == r -> intersect at one point
		// else if d > r -> no intersection
		if (dist <= radius) {

			if (getClosestIntersection(ray_origin, ray, intersection))
				return 1;

			return 0;
		}
			

		return 1;
	}

	// return the closest intersection
	int getClosestIntersection(const vec3_t *ray_origin, const vec3_t *ray, vec3_t *intersection) {
		if (!ray_origin || !ray || !intersection)
		{
			return 1;
		}

		vec3_t origin_to_ray_origin;
		if (vectorSubtract(ray_origin, &origin, &origin_to_ray_origin))
			return 1;

		// assume ray is normal
		
		float beta = 2*dotProduct(ray, &origin_to_ray_origin);
		float gamma = dotProduct(&origin_to_ray_origin, &origin_to_ray_origin) - radius * radius;
		float discriminant = beta * beta - 4 * gamma;
		
		/*
		float x_naught_to_a = (*ray_origin)[0] - origin[0];
		float y_naught_to_b = (*ray_origin)[1] - origin[1];
		float z_naught_to_c = (*ray_origin)[2] - origin[2];
		float beta = 2 * ((*ray)[0] * x_naught_to_a + (*ray)[1] * y_naught_to_b + (*ray)[2] * z_naught_to_c);
		float gamma = x_naught_to_a * x_naught_to_a + y_naught_to_b * y_naught_to_b + z_naught_to_c * z_naught_to_c - radius * radius;
		float discriminant = beta * beta - 4 * gamma;
		*/

		// t = (-beta +/- sqrt(betbeta - 4*gamma)/2
		float t;

		// no intersection if t is imaginary
		if (discriminant < 0)
			return 1;

		// pick the minimum positive t, and that defines the closest intersection
		t = (-beta - sqrt(discriminant)) / 2;
		if (t <= 0)
		{
			t = (-beta + sqrt(discriminant)) / 2;
		}
		
		(*intersection)[0] = (*ray_origin)[0] + (*ray)[0] * t;
		(*intersection)[1] = (*ray_origin)[1] + (*ray)[1] * t;
		(*intersection)[2] = (*ray_origin)[2] + (*ray)[2] * t;

		return 0;
	}
};