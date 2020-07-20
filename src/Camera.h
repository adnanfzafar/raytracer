#pragma once

#include "Vector4f.h"
#include "Matrix16f.h"

class Camera {
protected:
	float fov;
	Matrix16f *viewMatrix;

public:
	Camera();
	Camera(const Vector4f& dir, const Vector4f& up, const Vector4f& origin, const float new_fov);

	int setViewMatrix(Vector4f* dir, Vector4f* up, Vector4f* origin);

	Matrix16f* getViewMatrix();

	float setFov(float newFov);
	float getFov();
	int getRight(Vector4f* v);
	int getUp(Vector4f* v);
	int getEye(Vector4f* v);
	int getOrigin(Vector4f* v);
};