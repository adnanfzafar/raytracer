#pragma once

#include <math.h>
#include <string>

#include <xmmintrin.h> // SSE
#include <pmmintrin.h> // SSE3

#define CONST_PI 3.14159

class Vector4f {
protected:
	float v[4];

public:
	Vector4f();
	Vector4f(float a, float b, float c, float d);
	Vector4f(float a);
	Vector4f(Vector4f* a);
	~Vector4f();

	int set(float a, float b, float c, float d);
	int set(float a);
	int set(int index, float a);
	int set(Vector4f* a);
	int set(Vector4f& a);
	// make this safe?
	float get(int index);
	float* get();
	float magnitude3f();
	float magnitude();
	float normalize();
	float normalize3f();
	int add(Vector4f* a);
	int subtract(Vector4f* a);
	int multiply(float a);
	//x' = A * x
	int preMultiply(float* a_m);
	float innerProduct(Vector4f* a);
	float innerProduct(float* a);
	float innerProduct3f(Vector4f* a);
	// c = v x b
	// perform the 3d cross product using the first 3 elements of v and vector b, storing results in vector c, where c[w] = v[w]
	Vector4f* crossProduct3f(Vector4f* b);
	// v' = v projected onto b
	// perform the 3d vector projection of v onto b = (v . b)/|b| * b
	//if (vectorCopy(&projsonv, ray) || scaleVector(&projsonv, dotProduct(&s, ray)/vectorMagnitude(ray)))
	int projectOnto3f(Vector4f* b);

	int divide(float a);
};
