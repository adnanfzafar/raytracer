#pragma once

#include "Vector4f.h"
#include "color.h"

typedef enum {
	LIGHT_TYPE_NONE = 0,
	LIGHT_TYPE_AMBIENT,
	LIGHT_TYPE_POINT

} light_type_e;

typedef struct {
	vec4_t origin;
	vec4_t color;
	int type;
	float padding[3];  // structs passed to the GPU via buffer objects need to be multiples of vec4 (128 bytes) so pad the light
} light_s;

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

	int getData(light_s *light);
};
