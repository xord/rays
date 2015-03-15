// -*- c++ -*-
#pragma once
#ifndef __RAYS_MATRIX_H__
#define __RAYS_MATRIX_H__


#include <rays/defs.h>


namespace Rays
{


	struct Matrix
	{

		union
		{
			struct
			{
				float
					a1, a2, a3, a4,
					b1, b2, b3, b4,
					c1, c2, c3, c4,
					d1, d2, d3, d4;
			};
			float array[16];
		};

		Matrix (float value = 1);

		Matrix (
			float a1, float a2, float a3, float a4,
			float b1, float b2, float b3, float b4,
			float c1, float c2, float c3, float c4,
			float d1, float d2, float d3, float d4);

		Matrix (const float* elements, size_t size);

		Matrix dup () const;

		Matrix& reset (float value = 1);

		Matrix& reset (
			float a1, float a2, float a3, float a4,
			float b1, float b2, float b3, float b4,
			float c1, float c2, float c3, float c4,
			float d1, float d2, float d3, float d4);

		Matrix& reset (const float* elements, size_t size);

		float& at (int row, int column);

		float  at (int row, int column) const;

		float& operator [] (int index);

		float  operator [] (int index) const;

	};// Matrix


}// Rays


#endif//EOH
