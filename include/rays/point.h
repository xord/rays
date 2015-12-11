// -*- c++ -*-
#pragma once
#ifndef __RAYS_POINT_H__
#define __RAYS_POINT_H__


#include <rays/defs.h>


namespace Rays
{


	struct Coord2
	{

		union
		{
			struct {coord x, y;};
			coord array[2];
		};

		Coord2& reset (coord value = 0);

		Coord2& reset (coord x, coord y);

		      coord& operator [] (size_t index);

		const coord& operator [] (size_t index) const;

	};// Coord2


	struct Coord3
	{

		union
		{
			struct {coord x, y, z;};
			coord array[3];
		};

		Coord3& reset (coord value = 0);

		Coord3& reset (coord x, coord y, coord z = 0);

		      coord& operator [] (size_t index);

		const coord& operator [] (size_t index) const;

	};// Coord3


	struct Point : public Coord3
	{

		Point (coord value = 0);

		Point (coord x, coord y, coord z = 0);

		Point dup () const;

		Point& move_to (coord x, coord y, coord z = 0);

		Point& move_to (const Point& point);

		Point& move_by (coord x, coord y, coord z = 0);

		Point& move_by (const Point& point);

		void rotate (float angle);

		coord length () const;

		Point normal () const;

		void  normalize ();

		String inspect () const;

		Point operator - () const;

		Point& operator += (coord        rhs);

		Point& operator += (const Point& rhs);

		Point& operator -= (coord        rhs);

		Point& operator -= (const Point& rhs);

		Point& operator *= (coord        rhs);

		Point& operator *= (const Point& rhs);

		Point& operator /= (coord        rhs);

		Point& operator /= (const Point& rhs);

		friend bool operator == (const Point& lhs, const Point& rhs);

		friend bool operator != (const Point& lhs, const Point& rhs);

		friend Point operator + (coord        lhs, const Point& rhs);

		friend Point operator + (const Point& lhs, coord        rhs);

		friend Point operator + (const Point& lhs, const Point& rhs);

		friend Point operator - (coord        lhs, const Point& rhs);

		friend Point operator - (const Point& lhs, coord        rhs);

		friend Point operator - (const Point& lhs, const Point& rhs);

		friend Point operator * (coord        lhs, const Point& rhs);

		friend Point operator * (const Point& lhs, coord        rhs);

		friend Point operator * (const Point& lhs, const Point& rhs);

		friend Point operator / (coord        lhs, const Point& rhs);

		friend Point operator / (const Point& lhs, coord        rhs);

		friend Point operator / (const Point& lhs, const Point& rhs);

	};// Point


}// Rays


#endif//EOH
