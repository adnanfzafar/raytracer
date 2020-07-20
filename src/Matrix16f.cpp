#include "Matrix16f.h"

Matrix16f::Matrix16f()
{
	memset(&m[0], 0, sizeof(m));
}
Matrix16f::Matrix16f(float* f)
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

Matrix16f::~Matrix16f() {}

float* Matrix16f::get() { return &m[0]; }

int Matrix16f::getCol(int col, Vector4f* v)
{
	if (!v || col < 0 || col > 3)
	{
		return 1;
	}

	return v->set(m[col], m[col + 4], m[col + 8], m[col + 12]);
}

int Matrix16f::set(float* f)
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

int Matrix16f::setRow(int row, Vector4f* v)
{
	if (!v || row < 0 || row > 3)
		return 1;

	memcpy(&m[row * 4], v->get(), sizeof(float[4]));

	return 0;
}

int Matrix16f::setCol(int col, Vector4f* v)
{
	if (!v || col < 0 || col > 3)
		return 1;

	float* f = v->get();

	m[col] = f[0];
	m[col + 4] = f[1];
	m[col + 8] = f[2];
	m[col + 12] = f[3];

	return 0;
}


int Matrix16f::add(Matrix16f* a)
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

int Matrix16f::subtract(Matrix16f* a)
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
int Matrix16f::preMultiply(Matrix16f* a)
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
int Matrix16f::postMultiply(Matrix16f* a)
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
			Vector4f m_row(m[j * 4], m[j * 4 + 1], m[j * 4 + 2], m[j * 4 + 3]);
			m_prime[j * 4 + i] = m_row.innerProduct(&a_col);
		}
	}

	memcpy(&m, &m_prime, sizeof(m));

	return 0;
}
