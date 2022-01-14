#include "World.h"

World::World() {
	spheres = new std::vector<Sphere*>();
	lights = new std::vector<Light*>();
}

World::~World() {
	delete[] spheres;
	delete[] lights;
}

std::vector<Sphere*>* World::getSpheres() { return spheres; }
std::vector<Light*>* World::getLights() { return lights; }

bool World::isEmpty() {
	return (spheres->size() == 0);
}

bool World::isLit() {
	return (lights->size() == 0);
}

int World::update(long& ticks) {
	// animate the scene

	ticks++;

	// TODO: put proper physics in for objects
	if (isEmpty())
		return 1;

	spheres->at(0)->getOrigin()->get()[0] = 30 * sin(ticks / 100.0f);
	spheres->at(0)->getOrigin()->get()[1] = 30 * cos(ticks / 100.0f);
	//spheres->at(0)->getOrigin()->get()[2] = 40 * cos(ticks++ / 150.0f);

	lights->at(0)->getOrigin()->get()[1] = 1500 * sin(ticks / 75.0f);
	lights->at(0)->getOrigin()->get()[2] = 1500 * cos(ticks / 75.0f);

	return 0;

}

int World::genSphereBuffer(void *buffer, int len) {
	if (!buffer || (len < sizeof(sphere_s)* spheres->size()))
	{
		return 1;
	}

	sphere_s* sphere_buffer = (sphere_s *)buffer;
	
	int numSpheres = 0;
	memset(buffer, 0, len);
	for (auto s : *spheres)
	{
		if(s->getData(&(sphere_buffer[numSpheres]), sizeof(sphere_s)) )
			continue;

		numSpheres++;
	}


	return 0;
}

int World::genLightBuffer(void* buffer, int len) {
	if (!buffer || (len < sizeof(light_s) * lights->size()))
	{
		return 1;
	}

	light_s* light_buffer = (light_s*)buffer;

	int numLights = 0;
	memset(buffer, 0, len);
	for (auto light : *lights)
	{
		if (light->getData(&(light_buffer[numLights])))
			continue;

		numLights++;
	}


	return 0;
}