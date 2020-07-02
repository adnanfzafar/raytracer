#pragma once

#include <math.h>
#include <string>

#include <xmmintrin.h> // SSE
#include <pmmintrin.h> // SSE3

#define CONST_PI 3.14159

//class Matrix16f;


class Vector4f {
protected:
	float v[4];

public:
	Vector4f() 
	{
		memset(&v[0], 0, sizeof(v));
	}
	
	Vector4f(float a, float b, float c, float d)
	{
		v[0] = a;
		v[1] = b;
		v[2] = c;
		v[3] = d;
	}
	Vector4f(float a)
	{
		v[0] = a;
		v[1] = v[0];
		v[2] = v[1];
		v[3] = v[2];
	}
	Vector4f(Vector4f *a)
	{
		memcpy(&v[0], a->get(), sizeof(v));
	}

	~Vector4f() {}

	int set(float a, float b, float c, float d)
	{
		v[0] = a;
		v[1] = b;
		v[2] = c;
		v[3] = d;

		return 0;
	}

	int set(float a)
	{
		/*
		v[0] = a;
		v[1] = v[0];
		v[2] = v[1];
		v[3] = v[2];
		*/
		__m128 ra;
		ra = _mm_set1_ps(a);
		_mm_storeu_ps((float*)&v, ra);
		

		return 0;
	}

	int set(int index, float a)
	{
		v[index] = a;
		return 0;
	}

	int set(Vector4f* a)
	{
		if (!a)
			return 1;

		v[0] = a->get()[0];
		v[1] = a->get()[1];
		v[2] = a->get()[2];
		v[3] = a->get()[3];

		return 0;
	}

	int set(Vector4f& a)
	{
		v[0] = a.get()[0];
		v[1] = a.get()[1];
		v[2] = a.get()[2];
		v[3] = a.get()[3];

		return 0;
	}

	// make this safe?
	float get(int index)
	{
		return v[index];
	}

	float* get() {
		return &v[0];
	}

	float magnitude3f()
	{
		return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	}

	float magnitude()
	{
		return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3]*v[3]);
	}

	float normalize() {
		float magnitude = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3]);
		/*
		v[0] /= magnitude;
		v[1] /= magnitude;
		v[2] /= magnitude;
		v[3] /= magnitude;
		*/
		multiply(1/magnitude);

		return magnitude;
	}

	float normalize3f() {
		float magnitude = magnitude3f();
		/*
		v[0] /= magnitude;
		v[1] /= magnitude;
		v[2] /= magnitude;
		v[3] /= magnitude;
		*/
		multiply(1 / magnitude);

		return magnitude;
	}

	int add(Vector4f* a)
	{
		if (!a)
			return 0;

		/*
		v[0] += a->get()[0];
		v[1] += a->get()[1];
		v[2] += a->get()[2];
		v[3] += a->get()[3];
		*/

		__m128 ra, rb, rc;
		ra = _mm_loadu_ps((float*)a->get());
		rb = _mm_loadu_ps((float*)&v);
		rc = _mm_add_ps(ra, rb);
		_mm_storeu_ps((float*)&v, rc);

		return 0;
	}

	int subtract(Vector4f* a)
	{
		if (!a)
			return 0;

		/*
		v[0] -= a->get()[0];
		v[1] -= a->get()[1];
		v[2] -= a->get()[2];
		v[3] -= a->get()[3];
		*/
		__m128 ra, rb, rc;
		ra = _mm_loadu_ps((float*)a->get());
		rb = _mm_loadu_ps((float*)&v);
		rc = _mm_sub_ps(rb, ra);
		_mm_storeu_ps((float*)&v, rc);

		return 0;
	}

	int multiply(float a)
	{
		/*
		v[0] *= a;
		v[1] *= a;
		v[2] *= a;
		v[3] *= a;
		*/

		__m128 ra, rb, rc;
		ra = _mm_load_ps1((float*)&a);
		rb = _mm_loadu_ps((float*)&v);
		rc = _mm_mul_ps(ra, rb);
		_mm_storeu_ps((float*)&v, rc);

		return 0;
	}

	//x' = A * x
	int preMultiply(float* a_m)
	{
		if (!a_m)
			return 1;

		float v_prime[4];
		//float* a_m = a->get();

		// for each row in A
		for (int j = 0; j < 4; j++)
		{
			// dot v with jth row in A to get the element in row j		
			Vector4f a_row(a_m[j * 4], a_m[j * 4 + 1], a_m[j * 4 + 2], a_m[j * 4 + 3]);
			v_prime[j] = innerProduct(&a_row);
		}

		memcpy(&v, &v_prime, sizeof(v));

		return 0;
	}

	float innerProduct(Vector4f* a)
	{
		if (!a)
			return 0;

		float* f = a->get();

		return v[0] * f[0] + v[1] * f[1] + v[2] * f[2] + v[3] * f[3];

		/*
		float sum;

		__m128 ra, rb, rc;
		ra = _mm_load_ps1((float*)a->get());
		rb = _mm_loadu_ps((float*)&v);
		rc = _mm_mul_ps(ra, rb);
		// accumulate values in all elements into a single register and return
		rc = _mm_hadd_ps(rc, rc); // horizontally add
		rc = _mm_hadd_ps(rc, rc);
		_mm_store_ss(&sum, rc);
		
		return sum;
		*/
	}

	// c = v x b
	// perform the 3d cross product using the first 3 elements of v and vector b, storing results in vector c, where c[w] = v[w]
	Vector4f *crossProduct3f(Vector4f* b)
	{
		if (!b)
			return 0;

		float* f = b->get();
		return new Vector4f(v[1] * f[2] - v[2] * f[1], v[2] * f[0] - v[0] * f[2], v[0] * f[1] - v[1] * f[0], v[3]);

	}

	// v' = v projected onto b
	// perform the 3d vector projection of v onto b = (v . b)/|b| * b
	//if (vectorCopy(&projsonv, ray) || scaleVector(&projsonv, dotProduct(&s, ray)/vectorMagnitude(ray)))
	int projectOnto3f(Vector4f* b)
	{
		if (!b)
			return 0;

		float scalar = innerProduct(b) / b->magnitude3f();

		return (set(b) || multiply(scalar));
		
	}

	int divide(float a)
	{
		v[0] /= a;
		v[1] /= a;
		v[2] /= a;
		v[3] /= a;

		return 0;
	}

};

class Matrix16f {
protected:
	float m[16];

public:
	Matrix16f() 
	{
		memset(&m[0], 0, sizeof(m));
	}
	Matrix16f(float* f)
	{
		if (f)
		{
			for (int i = 0; i < 4; i++)
			{
				m[4 * i] = f[4 * i];
				m[4 * i + 1] = f[4 * i + 1];
				m[4 * i + 2] = f[4 * i + 2];
				m[4 * i + 3] = f[4 * i + 3];
			}
		}
	}

	~Matrix16f() {}

	float* get() { return &m[0]; }

	int set(float* f)
	{
		if (!f)
			return 1;

		for (int i = 0; i < 4; i++)
		{
			m[4 * i] = f[4 * i];
			m[4 * i + 1] = f[4 * i + 1];
			m[4 * i + 2] = f[4 * i + 2];
			m[4 * i + 3] = f[4 * i + 3];
		}

		return 0;
	}

	int setRow(int row, Vector4f* v)
	{
		if (!v || row < 0 || row > 3)
			return 1;

		memcpy(&m[row * 4], v->get(), sizeof(float[4]));

		return 0;
	}

	int setCol(int col, Vector4f* v)
	{
		if (!v || col < 0 || col > 3)
			return 1;

		float* f = v->get();

		m[col] = f[0];
		m[col+4] = f[1];
		m[col+8] = f[2];
		m[col+12] = f[3];

		return 0;
	}


	int add(Matrix16f* a)
	{
		if (!a)
			return 1;

		float* f = a->get();

		for (int i = 0; i < 4; i++)
		{
			m[4 * i] += f[4 * i];
			m[4 * i + 1] += f[4 * i + 1];
			m[4 * i + 2] += f[4 * i + 2];
			m[4 * i + 3] += f[4 * i + 3];
		}

		return 0;
	}

	int subtract(Matrix16f* a)
	{
		if (!a)
			return 1;

		float* f = a->get();

		for (int i = 0; i < 4; i++)
		{
			m[4 * i] -= f[4 * i];
			m[4 * i + 1] -= f[4 * i + 1];
			m[4 * i + 2] -= f[4 * i + 2];
			m[4 * i + 3] -= f[4 * i + 3];
		}

		return 0;
	}

	//M' = A * M
	int preMultiply(Matrix16f* a)
	{
		if (!a)
			return 1;

		float m_prime[16];
		float* a_m = a->get();
		
		// for each column in M
		for (int i = 0; i < 4; i++)
		{
			Vector4f m_col(m[i], m[i + 4], m[i + 8], m[i + 12]);

			// for each row in A
			for (int j = 0; j < 4; j++)
			{
				// dot ith col in M with jth row in A to get the element in row j, column i		
				Vector4f a_row(a_m[j * 4], a_m[j * 4 + 1], a_m[j * 4 + 2], a_m[j * 4 + 3]);
				m_prime[j * 4 + i] = a_row.innerProduct(&m_col);
			}
		}

		memcpy(&m, &m_prime, sizeof(m));

		return 0;
	}

	//M' = M*A
	int postMultiply(Matrix16f* a)
	{
		if (!a)
			return 1;

		float m_prime[16];
		float* a_m = a->get();

		// for each column in A
		for (int i = 0; i < 4; i++)
		{
			Vector4f a_col(a_m[i], a_m[i + 4], a_m[i + 8], a_m[i + 12]);

			// for each row in M
			for (int j = 0; j < 4; j++)
			{
				// dot ith col in A with jth row in M to get the element in row j, column i
				Vector4f m_row(m[j*4], m[j*4+1], m[j*4+2], m[j*4+3]);
				m_prime[j * 4 + i] = m_row.innerProduct(&a_col);
			}
		}

		memcpy(&m, &m_prime, sizeof(m));

		return 0;
	}
};
