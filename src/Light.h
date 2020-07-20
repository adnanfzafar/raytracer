#pragma once

#include "Vector4f.h"
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
	int setOrigin(Vector4f& o);
	int setOrigin(Vector4f* o);
	Vector4f* getOrigin();
	
	int setType(int newType);
	int getType();

	Vector4f* getColor();
};
