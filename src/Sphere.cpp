#include "Sphere.h"

int Sphere::setRadius(float r) {
	if (r <= 0) {
		radius = 10;
		radius_squared = radius * radius;
		return 1;
	}
	radius = r;
	radius_squared = radius * radius;
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
	/*
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
	*/

	
	if (!ray_origin || !ray || !hit_record)
		return 1;


	// Let s = origin - ray_origin
	Vector4f s(origin);
	if (s.subtract((Vector4f*)ray_origin))
		return 1;


	// project s onto the ray direction. the magnitude of this projection is the closest approach of the ray to the sphere origin

	// since the ray direction is normalized, calculating this projection and finding magnitude is equivalent to the dot product
	float tca = s.innerProduct3f((Vector4f*)ray);
	float l2oc = s.innerProduct3f(&s);

	// if the point originates outside of the sphere and points away, it doesn't intersect
	if ((l2oc > radius_squared) && (tca < 0))
		return 1;

	// determine the half cord distance squared. if this distance is less than 0, that indicates that the ray missed the sphere
	float t2hc = radius_squared - l2oc + tca * tca;
	if (t2hc < 0)
		return 1;

	// find the intersection point
	float t;

	// if the ray originated within the sphere, t = tca +sqrt(t2hc)
	if (l2oc < radius_squared)
	{
		t = tca + sqrt(t2hc);
	}
	// if the ray originated outside the sphere, t = tca - sqrt(t2hc)
	else if(l2oc > radius_squared)
	{
		t = tca - sqrt(t2hc);
	}

	if (hit_record->point.set((Vector4f*)ray) || hit_record->point.multiply(t) || hit_record->point.add((Vector4f*)ray_origin) || getNormal(&(hit_record->point), &(hit_record->normal)) || hit_record->diffuseColor.set(diffuseColor) || hit_record->specularColor.set(specularColor) || !(hit_record->material = &material))
		return 1;

	return 0;
	
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

int Sphere::getData(void *dest, int len) {
	if (!dest || (len < sizeof(sphere_s)))
		return 1;

	sphere_s* sphere = (sphere_s*)dest;

	memcpy(&(sphere->origin), origin.get(), sizeof(vec4_t));
	memcpy(&(sphere->diffuse_color), diffuseColor.get(), sizeof(vec4_t));
	memcpy(&(sphere->specular_color), specularColor.get(), sizeof(vec4_t));
	memcpy(&(sphere->material), &material, sizeof(material_s));
	sphere->radius = radius;


	return 0;
}
