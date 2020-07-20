#include "World.h"

World::World() {
	primitives = new std::vector<Primitive*>();
	lights = new std::vector<Light*>();
}

World::~World() {
	delete[] primitives;
	delete[] lights;
}

std::vector<Primitive*>* World::getPrimitives() { return primitives; }
std::vector<Light*>* World::getLights() { return lights; }

bool World::isEmpty() {
	return (primitives->size() == 0);
}

bool World::isLit() {
	return (lights->size() == 0);
}

int World::update(long& ticks) {
	// animate the scene

	// TODO: put proper physics in for objects
	if (isEmpty())
		return 1;

	primitives->at(0)->getOrigin()->get()[0] = 30 * sin(ticks++ / 100.0f);
	primitives->at(0)->getOrigin()->get()[1] = 30 * cos(ticks++ / 100.0f);
	//primitives->at(0)->getOrigin()->get()[2] = 40 * cos(ticks++ / 150.0f);

	return 0;

}