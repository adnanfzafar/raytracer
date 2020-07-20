#include "PointLight.h"

PointLight::PointLight(const Vector4f* newOrigin, const Vector4f* newColor)
{
	type = LIGHT_TYPE_POINT;

	origin.set((Vector4f*)newOrigin);
	color.set((Vector4f*)newColor);
}