#pragma once

#include <vector>
#include "primitives.h"

typedef struct {
	vec3_t point;
	vec3_t normal;
	float t;
	// TODO: surface properties
} hit_list_t;

class World {
protected:
	std::vector<Primitive*>* primitives;

public:
	World() {
		primitives = new std::vector<Primitive*>();
	}

	~World() {
		delete[] primitives;
	}

	std::vector<Primitive*>* getPrimitives() { return primitives; }

	bool isEmpty() {
		return (primitives->size() == 0);
	}

	int update(long &ticks) {
		// animate the scene
		
		// TODO: put proper physics in for objects
		if (isEmpty())
			return 1;

		*(primitives->at(0)->getOrigin())[0] = 30 * sin(ticks++ / 100);

		return 0;
		
	}
};