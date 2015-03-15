#include "rays/matrix.h"


namespace Rays
{


	Matrix::Matrix (float value)
	{
		reset(value);
	}

	Matrix::Matrix (
		float a1, float a2, float a3, float a4,
		float b1, float b2, float b3, float b4,
		float c1, float c2, float c3, float c4,
		float d1, float d2, float d3, float d4)
	{
		reset(a1, a2, a3, a4, b1, b2, b3, b4, c1, c2, c3, c4, d1, d2, d3, d4);
	}

	Matrix::Matrix (const float* elements, size_t size)
	{
		reset(elements, size);
	}

	Matrix
	Matrix::dup () const
	{
		return *this;
	}

	Matrix&
	Matrix::reset (float value)
	{
		return reset(
			value, 0, 0, 0,
			0, value, 0, 0,
			0, 0, value, 0,
			0, 0, 0, value);
	}

	Matrix&
	Matrix::reset (
		float a1, float a2, float a3, float a4,
		float b1, float b2, float b3, float b4,
		float c1, float c2, float c3, float c4,
		float d1, float d2, float d3, float d4)
	{
		this->a1 = a1; this->a2 = a2; this->a3 = a3; this->a4 = a4;
		this->b1 = b1; this->b2 = b2; this->b3 = b3; this->b4 = b4;
		this->c1 = c1; this->c2 = c2; this->c3 = c3; this->c4 = c4;
		this->d1 = d1; this->d2 = d2; this->d3 = d3; this->d4 = d4;
		return *this;
	}

	Matrix&
	Matrix::reset (const float* elements, size_t size)
	{
		if (size != 16) return *this;
		float* e = array;
		for (int i = 0; i < 16; ++i) *e++ = *elements++;
		return *this;
	}

	float&
	Matrix::at (int row, int column)
	{
		return array[column * 4 + row];
	}

	float
	Matrix::at (int row, int column) const
	{
		return const_cast<Matrix*>(this)->at(row, column);
	}

	float&
	Matrix::operator [] (int index)
	{
		return array[index];
	}

	float
	Matrix::operator [] (int index) const
	{
		return const_cast<Matrix*>(this)->operator[](index);
	}


}// Rays
