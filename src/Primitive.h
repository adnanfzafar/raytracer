#pragma once

#include <iostream>
#include "Vector4f.h"
#include "material.h"
#include "hitrecord.h"

class Primitive {
protected:
	Vector4f origin;
	Vector4f diffuseColor;
	Vector4f specularColor;
	material_s material;
public:


	int setOrigin(Vector4f& o);
	int setOrigin(Vector4f* o);
	Vector4f* getOrigin();

	int setDiffuseColor(Vector4f& c);
	Vector4f* getDiffuseColor();

	int setSpecularColor(Vector4f& c);
	Vector4f* getSpecularColor();

	virtual int intersect(const Vector4f* ray_origin, const Vector4f* ray, hit_record_s *hit_record) = 0;
	virtual int getNormal(const Vector4f* point, Vector4f* normal) = 0;
};

