#pragma once

#include "math.h"
#include "color.h"

typedef enum {
	LIGHT_TYPE_NONE = 0,
	LIGHT_TYPE_AMBIENT,
	LIGHT_TYPE_POINT

} light_type_e;

class Light {
protected:
	Vector4f origin;
	Vector4f color;

	int type;
	
public:
	int setOrigin(Vector4f& o) {

		return (origin.set(o));

	}

	int setOrigin(Vector4f* o) {

		return (origin.set(o));

	}

	Vector4f* getOrigin() { return &origin; }
	
	int setType(int newType)
	{
		type = newType;
		return type;
	}

	int getType() { return type; }

	Vector4f* getColor() { return &color; };
};

class PointLight : public Light {
public:
	PointLight(const Vector4f *newOrigin, const Vector4f *newColor) 
	{
		type = LIGHT_TYPE_POINT;
		
		origin.set((Vector4f*)newOrigin);
		color.set((Vector4f*)newColor);
	}

};