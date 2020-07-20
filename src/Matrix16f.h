#pragma once

#include "Vector4f.h"

#include <string>

#include <xmmintrin.h> // SSE
#include <pmmintrin.h> // SSE3

class Matrix16f {
protected:
	float m[16];

public:
	Matrix16f();
	Matrix16f(float* f);
	~Matrix16f();

	float* get();
	int getCol(int col, Vector4f* v);
	int set(float* f);
	int setRow(int row, Vector4f* v);
	int setCol(int col, Vector4f* v);
	int add(Matrix16f* a);
	int subtract(Matrix16f* a);
	//M' = A * M
	int preMultiply(Matrix16f* a);
	//M' = M*A
	int postMultiply(Matrix16f* a);
};