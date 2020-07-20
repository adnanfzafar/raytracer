#include "Sphere.h"

int Sphere::setRadius(float r) {
	if (r <= 0) {
		radius = 10;
		return 1;
	}
	radius = r;
	return 0;
}

Sphere::Sphere(float r, const Vector4f* o, Vector4f* newDiffuseColor, Vector4f* newSpecularColor, const material_s* newMaterial) {
	setRadius(r);
	setOrigin((Vector4f*)o);
	setSpecularColor(*newSpecularColor);
	setDiffuseColor(*newDiffuseColor);
	memcpy(&material, newMaterial, sizeof(material_s));
}
Sphere::~Sphere() {};


int Sphere::getNormal(const Vector4f* point, Vector4f* normal)
{
	if (!point || !normal)
	{
		return 1;
	}
	if (normal->set((Vector4f*)point) || normal->subtract(&origin))
		return 1;

	normal->normalize3f();

	return 0;
}



int Sphere::intersect(const Vector4f* ray_origin, const Vector4f* ray, hit_record_s* hit_record) {

	//Vector4f intersection;

	if (!ray_origin || !ray || !hit_record)
		return 1;

	// Let s = origin - ray_origin
	Vector4f s(origin);
	if (s.subtract((Vector4f*)ray_origin))
		return 1;

	// Let projsonv = Project s onto v = (s . v)/|v| * v
	if (s.projectOnto3f((Vector4f*)ray))
		return 1;

	// Then let q' = end point of the projection in the global coordinate system: ray_origin + projsonv
	Vector4f q_prime((Vector4f*)ray_origin);
	if (q_prime.add(&s))
		return 1;

	// Then, the distance dist from the center of the sphere origin to the end of the global projection q' made by the ray is d = |origin - q'|
	Vector4f d(origin);
	if (d.subtract(&q_prime))
		return 1;
	//float dist = vectorMagnitude(&d);

	//std::cout << dist << std::endl;

	// If d < r -> intersect at 2 points
	// else if d == r -> intersect at one point
	// else if d > r -> no intersection
	if (d.magnitude3f() <= radius)
	{
		if (getClosestIntersection(ray_origin, ray, &(hit_record->point)) || getNormal(&(hit_record->point), &(hit_record->normal)) || hit_record->diffuseColor.set(diffuseColor) || hit_record->specularColor.set(specularColor) || !(hit_record->material = &material))
			return 1;

		return 0;
	}

	return 1;
}


// return the closest intersection
int Sphere::getClosestIntersection(const Vector4f* ray_origin, const Vector4f* ray, Vector4f* intersection) {
	if (!ray_origin || !ray || !intersection)
	{
		return 1;
	}

	Vector4f origin_to_ray_origin((Vector4f*)ray_origin);
	if (origin_to_ray_origin.subtract(&origin))
		return 1;

	// assume ray is normal

	float beta = 2 * origin_to_ray_origin.innerProduct((Vector4f*)ray);
	float gamma = origin_to_ray_origin.innerProduct(&origin_to_ray_origin) - radius * radius;
	float discriminant = beta * beta - 4 * gamma;

	// t = (-beta +/- sqrt(betbeta - 4*gamma)/2
	float t;

	// no intersection if t is imaginary
	if (discriminant < 0)
		return 1;

	// pick the minimum positive t, and that defines the closest intersection
	t = (-beta - sqrt(discriminant)) / 2;
	if (t <= 0)
	{
		t = (-beta + sqrt(discriminant)) / 2;
	}

	if (intersection->set((Vector4f*)ray) || intersection->multiply(t) || intersection->add((Vector4f*)ray_origin))
		return 1;

	return 0;
}
