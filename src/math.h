#pragma once

#include <math.h>
#include <string>

//__m128 _mm_add_ps(__m128 a, __m128 b)
#include <xmmintrin.h>

#define CONST_PI 3.14159

typedef float vec3_t[3];
typedef float vec4_t[4];
typedef float mat3x3_t[9];
typedef float mat4x4_t[16];

int vectorClear(vec3_t* v);

int vectorCopy(vec3_t* dst, const vec3_t* src);

int normalize(vec3_t* v);

float vectorMagnitude(const vec3_t* v);
int vectorSubtract(const vec3_t* a, const vec3_t* b, vec3_t* c);
int vectorAdd(const vec3_t* a, const vec3_t* b, vec3_t* c);
int vectorScale(const vec3_t* a, const float k, vec3_t* b);

int multMatVec(const mat3x3_t* T, vec3_t* v);

int scaleVector(vec3_t* v, float c);

float dotProduct(const vec3_t* a, const vec3_t* b);
int crossProduct(const vec3_t* a, const vec3_t* b, vec3_t* c);