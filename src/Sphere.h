#pragma once

#include "Primitive.h"

class Sphere : public Primitive {
protected:
	float radius;

public:
	int setRadius(float r);

	Sphere(float r, const Vector4f* o, Vector4f* newDiffuseColor, Vector4f* newSpecularColor, const material_s* newMaterial);
	~Sphere();

	int getNormal(const Vector4f* point, Vector4f* normal);
	int intersect(const Vector4f* ray_origin, const Vector4f* ray, hit_record_s* hit_record);
	// return the closest intersection
	int getClosestIntersection(const Vector4f* ray_origin, const Vector4f* ray, Vector4f* intersection);
};