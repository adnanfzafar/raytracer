#include "Primitive.h"

int Primitive::setOrigin(Vector4f& o)
{
	return (origin.set(o));
}

int Primitive::setOrigin(Vector4f* o)
{
	return (origin.set(o));
}

Vector4f* Primitive::getOrigin() { return &origin; }

int Primitive::setDiffuseColor(Vector4f& c)
{
	return (diffuseColor.set(c));
}
Vector4f* Primitive::getDiffuseColor() { return &diffuseColor; }

int Primitive::setSpecularColor(Vector4f& c)
{
	return (specularColor.set(c));
}
Vector4f* Primitive::getSpecularColor() { return &specularColor; }
