#pragma once

#include "Primitive.h"

typedef struct {
	vec4_t origin;
	vec4_t diffuse_color;
	vec4_t specular_color;
	material_s material;
	float radius;
	float padding[3]; // structs passed to the GPU via buffer objects need to be multiples of vec4 (128 bytes) so pad the sphere
} sphere_s;

class Sphere : public Primitive {
protected:
	float radius;
	float radius_squared;

public:
	int setRadius(float r);

	Sphere(float r, const Vector4f* o, Vector4f* newDiffuseColor, Vector4f* newSpecularColor, const material_s* newMaterial);
	~Sphere();

	int getNormal(const Vector4f* point, Vector4f* normal);
	int intersect(const Vector4f* ray_origin, const Vector4f* ray, hit_record_s* hit_record);
	// return the closest intersection
	int getClosestIntersection(const Vector4f* ray_origin, const Vector4f* ray, Vector4f* intersection);

	int getData(void *dest, int len);
};