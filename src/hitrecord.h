#pragma once

#include "math.h"
#include "color.h"

typedef struct {
	Vector4f point;
	Vector4f normal;
	Vector4f diffuseColor;
	Vector4f specularColor;
	material_s* material;
} hit_record_s;