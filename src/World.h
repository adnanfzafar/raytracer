#pragma once

#include <vector>
#include "Primitive.h"
#include "Sphere.h"
#include "Light.h"


class World {
protected:
	//std::vector<Primitive*> *primitives;
	std::vector<Sphere*> *spheres;
	std::vector<Light*> *lights;

public:
	World();
	~World();

	//std::vector<Primitive*> *getPrimitives();
	std::vector<Sphere*>* getSpheres();
	std::vector<Light*> *getLights();

	bool isEmpty();

	bool isLit();

	int update(long &ticks);

	// generates buffers needed for use by shaders
	int genSphereBuffer(void* buffer, int len);
	int genLightBuffer(void* buffer, int len);
};