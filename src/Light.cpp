#include "Light.h"


int Light::setOrigin(Vector4f& o) {

	return (origin.set(o));

}

int Light::setOrigin(Vector4f* o) {

	return (origin.set(o));

}

Vector4f* Light::getOrigin() { return &origin; }

int Light::setType(int newType)
{
	type = newType;
	return type;
}

int Light::getType() { return type; }

Vector4f* Light::getColor() { return &color; };

int Light::getData(light_s *light) {
	if (!light)
		return 1;

	memcpy(&(light->color), color.get(), sizeof(vec4_t));
	memcpy(&(light->origin), origin.get(), sizeof(vec4_t));
	light->type = type;

	return 0;
}