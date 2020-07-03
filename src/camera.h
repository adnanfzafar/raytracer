#pragma once

#include "math.h"

class Camera {
protected:
	float fov;
	Matrix16f *viewMatrix;

public:
	Camera() 
	{
		viewMatrix = new Matrix16f();
		
		// default the view matrix to the standard right hand view matrix
		float* f = viewMatrix->get();
		f[0] = 1;
		f[5] = 1;
		f[10] = 1;
		f[15] = 1;

		fov = 90;
	}

	Camera(const Vector4f& dir, const Vector4f& up, const Vector4f& origin, const float new_fov)
	{
		viewMatrix = new Matrix16f();
		
		if (setViewMatrix((Vector4f*)&dir, (Vector4f*)&up, (Vector4f*)&origin))
		{
			// default the view matrix to the standard right hand view matrix
			float* f = viewMatrix->get();
			memset(f, 0, sizeof(float[16]));
			f[0] = 1;
			f[5] = 1;
			f[10] = 1;
			f[15] = 1;
		}
		fov = new_fov;
	}

	int setViewMatrix(Vector4f* dir, Vector4f* up, Vector4f* origin)
	{
		if (!dir || !up || !origin || !viewMatrix)
			return 1;

		Vector4f *right = dir->crossProduct3f(up);

		viewMatrix->setCol(0, right);
		viewMatrix->setCol(1, up);
		viewMatrix->setCol(2, dir);
		viewMatrix->setCol(3, origin);

		delete right;

		return 0;
	}

	Matrix16f *getViewMatrix() { return viewMatrix; }

	float setFov(float newFov) 
	{ 
		fov = newFov; 
		return fov; 
	}
	float getFov() { return fov; }

	int getRight(Vector4f* v)
	{
		if (!v)
			return 1;

		return viewMatrix->getCol(0, v);
	}

	int getUp(Vector4f* v)
	{
		if (!v)
			return 1;

		return viewMatrix->getCol(1, v);
	}

	int getEye(Vector4f* v)
	{
		if (!v)
			return 1;

		return viewMatrix->getCol(2, v);
	}

	int getOrigin(Vector4f* v) 
	{
		if (!v)
			return 1;

		return viewMatrix->getCol(3, v);
	}

	
};