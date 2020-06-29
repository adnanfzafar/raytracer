#include "math.h"

int vectorClear(vec3_t* v) {
	if (!v)
		return 1;

	memset((void*)v, 0, sizeof(vec3_t));

	return 0;
}

int vectorCopy(vec3_t* dst, const vec3_t* src)
{
	if (!dst || !src)
		return 1;

	(*dst)[0] = (*src)[0];
	(*dst)[1] = (*src)[1];
	(*dst)[2] = (*src)[2];

	return 0;
}

int scaleVector(vec3_t* v, float c) {
	if (!v)
		return 1;

	(*v)[0] *= c;
	(*v)[1] *= c;
	(*v)[2] *= c;

	return 0;
}

float vectorMagnitude(const vec3_t* v) {
	if (!v)
		return 0;

	return sqrt(dotProduct(v, v));
}

int vectorSubtract(const vec3_t* a, const vec3_t* b, vec3_t* c) {
	if (!a || !b || !c)
		return 1;

	(*c)[0] = (*a)[0] - (*b)[0];
	(*c)[1] = (*a)[1] - (*b)[1];
	(*c)[2] = (*a)[2] - (*b)[2];

	/*
	__m128 ra, rb, rc;
	ra = _mm_loadu_ps((float*)a);
	rb = _mm_loadu_ps((float*)b);
	rc = _mm_sub_ps(ra, rb);
	_mm_storeu_ps((float*)c, rc);
	*/

	return 0;
}


int vectorAdd(const vec3_t* a, const vec3_t* b, vec3_t* c) {
	if (!a || !b || !c)
		return 1;

	(*c)[0] = (*a)[0] + (*b)[0];
	(*c)[1] = (*a)[1] + (*b)[1];
	(*c)[2] = (*a)[2] + (*b)[2];
	
	/*
	__m128 ra, rb, rc;
	ra = _mm_loadu_ps((float*)a);
	rb = _mm_loadu_ps((float*)b);
	rc = _mm_add_ps(ra, rb);
	_mm_storeu_ps((float*)c, rc);
	*/

	return 0;
}


int vectorScale(const vec3_t* a, const float k, vec3_t* b)
{
	if (!a || !b)
		return 1;

	(*b)[0] = (*a)[0] * k;
	(*b)[1] = (*a)[1] * k;
	(*b)[2] = (*a)[2] * k;

	return 0;
}

int normalize(vec3_t* v) {
	if (!v)
		return 1;

	float magnitude = vectorMagnitude(v);

	if (magnitude <= 0)
		return 1;

	(*v)[0] /= magnitude;
	(*v)[1] /= magnitude;
	(*v)[2] /= magnitude;

	return 0;
}


float dotProduct(const vec3_t* a, const vec3_t* b) {
	if (!a || !b)
		return 0;

	return ((*a)[0] * (*b)[0] + (*a)[1] * (*b)[1] + (*a)[2] * (*b)[2]);
}

int crossProduct(const vec3_t* a, const vec3_t* b, vec3_t* c)
{
	if (!a || !b || !c)
	{
		return 1;
	}

	(*c)[0] = (*a)[1] * (*b)[2] - (*a)[2] * (*b)[1];
	(*c)[1] = (*a)[2] * (*b)[0] - (*a)[0] * (*b)[2];
	(*c)[2] = (*a)[0] * (*b)[1] - (*a)[1] * (*b)[0];

	return 0;
}

int multMatVec(const mat3x3_t* T, vec3_t* v) {
	if (!T || !v)
	{
		return 1;
	}

	(*v)[0] = dotProduct((vec3_t*)&T[0], v);
	(*v)[1] = dotProduct((vec3_t*)&T[3], v);
	(*v)[2] = dotProduct((vec3_t*)&T[6], v);

	return 0;
}
