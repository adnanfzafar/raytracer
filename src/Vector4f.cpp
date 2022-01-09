#include "Vector4f.h"

Vector4f::Vector4f()
{
	memset(&v[0], 0, sizeof(v));
}

Vector4f::Vector4f(float a, float b, float c, float d)
{
	v[0] = a;
	v[1] = b;
	v[2] = c;
	v[3] = d;
}
Vector4f::Vector4f(float a)
{
	v[0] = a;
	v[1] = v[0];
	v[2] = v[1];
	v[3] = v[2];
}
Vector4f::Vector4f(Vector4f* a)
{
	memcpy(&v[0], a->get(), sizeof(v));
}

Vector4f::~Vector4f() {}

int Vector4f::set(float a, float b, float c, float d)
{
	v[0] = a;
	v[1] = b;
	v[2] = c;
	v[3] = d;

	return 0;
}

int Vector4f::set(float a)
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

int Vector4f::set(int index, float a)
{
	v[index] = a;
	return 0;
}

int Vector4f::set(Vector4f* a)
{
	if (!a)
		return 1;

	v[0] = a->get()[0];
	v[1] = a->get()[1];
	v[2] = a->get()[2];
	v[3] = a->get()[3];

	return 0;
}

int Vector4f::set(Vector4f& a)
{
	v[0] = a.get()[0];
	v[1] = a.get()[1];
	v[2] = a.get()[2];
	v[3] = a.get()[3];

	return 0;
}

// make this safe?
float Vector4f::get(int index)
{
	return v[index];
}

float* Vector4f::get() {
	return &(v[0]);
}

float Vector4f::magnitude3f()
{
	return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

float Vector4f::magnitude()
{
	return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3]);
}

float Vector4f::normalize() {
	float magnitude = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3]);
	/*
	v[0] /= magnitude;
	v[1] /= magnitude;
	v[2] /= magnitude;
	v[3] /= magnitude;
	*/
	multiply(1 / magnitude);

	return magnitude;
}

float Vector4f::normalize3f() {
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

int Vector4f::add(Vector4f* a)
{
	if (!a)
		return 0;

	
	v[0] += a->get()[0];
	v[1] += a->get()[1];
	v[2] += a->get()[2];
	v[3] += a->get()[3];
	
	/*
	__m128 ra, rb, rc;
	ra = _mm_loadu_ps((float*)a->get());
	rb = _mm_loadu_ps((float*)&v);
	rc = _mm_add_ps(ra, rb);
	_mm_storeu_ps((float*)&v, rc);
	*/

	return 0;
}

int Vector4f::subtract(Vector4f* a)
{
	if (!a)
		return 0;

	v[0] -= a->get()[0];
	v[1] -= a->get()[1];
	v[2] -= a->get()[2];
	v[3] -= a->get()[3];
	
	/*
	__m128 ra, rb, rc;
	ra = _mm_loadu_ps((float*)a->get());
	rb = _mm_loadu_ps((float*)&v);
	rc = _mm_sub_ps(rb, ra);
	_mm_storeu_ps((float*)&v, rc);
	*/

	return 0;
}

int Vector4f::multiply(float a)
{
	v[0] *= a;
	v[1] *= a;
	v[2] *= a;
	v[3] *= a;
	
	/*
	__m128 ra, rb, rc;
	ra = _mm_load_ps1((float*)&a);
	rb = _mm_loadu_ps((float*)&v);
	rc = _mm_mul_ps(ra, rb);
	_mm_storeu_ps((float*)&v, rc);
	*/

	return 0;
}

//x' = A * x
int Vector4f::preMultiply(float* a_m)
{
	if (!a_m)
		return 1;

	float v_prime[4];
	//float* a_m = a->get();

	// for each row in A
	for (int j = 0; j < 4; j++)
	{
		// dot v with jth row in A to get the element in row j		
		//Vector4f a_row(a_m[j * 4], a_m[j * 4 + 1], a_m[j * 4 + 2], a_m[j * 4 + 3]);
		//v_prime[j] = innerProduct(&a_row);
		v_prime[j] = innerProduct(&a_m[j * 4]);
	}

	memcpy(&v, &v_prime, sizeof(v));

	return 0;
}

float Vector4f::innerProduct(Vector4f* a)
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

float Vector4f::innerProduct(float* a)
{
	if (!a)
		return 0;

	return v[0] * a[0] + v[1] * a[1] + v[2] * a[2] + v[3] * a[3];

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

float Vector4f::innerProduct3f(Vector4f* a)
{
	if (!a)
		return 0;

	float* f = a->get();

	return v[0] * f[0] + v[1] * f[1] + v[2] * f[2];
}

// c = v x b
// perform the 3d cross product using the first 3 elements of v and vector b, storing results in vector c, where c[w] = v[w]
Vector4f* Vector4f::crossProduct3f(Vector4f* b)
{
	if (!b)
		return 0;

	float* f = b->get();
	return new Vector4f(v[1] * f[2] - v[2] * f[1], v[2] * f[0] - v[0] * f[2], v[0] * f[1] - v[1] * f[0], v[3]);

}

// v' = v projected onto b
// perform the 3d vector projection of v onto b = (v . b)/|b| * b
//if (vectorCopy(&projsonv, ray) || scaleVector(&projsonv, dotProduct(&s, ray)/vectorMagnitude(ray)))
int Vector4f::projectOnto3f(Vector4f* b)
{
	if (!b)
		return 0;

	float scalar = innerProduct(b) / b->magnitude3f();

	return (set(b) || multiply(scalar));

}

int Vector4f::divide(float a)
{
	v[0] /= a;
	v[1] /= a;
	v[2] /= a;
	v[3] /= a;

	return 0;
}
