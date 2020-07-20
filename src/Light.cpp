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