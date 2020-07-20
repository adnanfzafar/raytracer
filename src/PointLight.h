#pragma once

#include "Light.h"

class PointLight : public Light {
public:
	PointLight(const Vector4f* newOrigin, const Vector4f* newColor);
};