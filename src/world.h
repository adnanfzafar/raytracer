#pragma once

#include <vector>
#include "primitives.h"


typedef struct {
	Vector4f point;
	Vector4f normal;
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

		primitives->at(0)->getOrigin()->get()[0] = 30 * sin(ticks++ / 100.0f);
		primitives->at(0)->getOrigin()->get()[1] = 30 * cos(ticks++ / 100.0f);
		//primitives->at(0)->getOrigin()->get()[2] = 40 * cos(ticks++ / 150.0f);

		return 0;
		
	}
};