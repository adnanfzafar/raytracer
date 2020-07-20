#pragma once

#include <vector>
#include "Primitive.h"
#include "Light.h"


class World {
protected:
	std::vector<Primitive*>* primitives;
	std::vector<Light*>* lights;

public:
	World();
	~World();

	std::vector<Primitive*>* getPrimitives();
	std::vector<Light*>* getLights();

	bool isEmpty();

	bool isLit();

	int update(long& ticks);
};