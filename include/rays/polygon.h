// -*- c++ -*-
#pragma once
#ifndef __RAYS_POLYGON_H__
#define __RAYS_POLYGON_H__


#include <xot/pimpl.h>
#include <rays/point.h>
#include <rays/bounds.h>


namespace Rays
{


	class Polygon
	{

		public:

			Polygon ();

			Polygon (const Point* points, size_t size, bool loop = true);

			~Polygon ();

			Polygon dup () const;

			void add (const Point* points, size_t size, bool loop = true);

			void add_hole (const Point* points, size_t size);

			void clear ();

			operator bool () const;

			bool operator ! () const;

			Polygon& operator -= (const Polygon& rhs);

			Polygon& operator &= (const Polygon& rhs);

			Polygon& operator |= (const Polygon& rhs);

			Polygon& operator ^= (const Polygon& rhs);

			friend Polygon operator - (const Polygon& lhs, const Polygon& rhs);

			friend Polygon operator & (const Polygon& lhs, const Polygon& rhs);

			friend Polygon operator | (const Polygon& lhs, const Polygon& rhs);

			friend Polygon operator ^ (const Polygon& lhs, const Polygon& rhs);

			struct Data;

			Xot::PSharedImpl<Data> self;

	};// Polygon


	Polygon create_line (coord x1, coord y1, coord x2, coord y2);

	Polygon create_line (const Point& p1, const Point& p2);

	Polygon create_line  (const Point* points, size_t size, bool loop = false);


	Polygon create_rect (
		coord x, coord y, coord width, coord height,
		coord round = 0, uint nsegment = 0);

	Polygon create_rect (
		coord x, coord y, coord width, coord height,
		coord round_left_top,    coord round_right_top,
		coord round_left_bottom, coord round_right_bottom,
		uint nsegment = 0);

	Polygon create_rect (
		const Bounds& bounds,
		coord round = 0, uint nsegment = 0);

	Polygon create_rect (
		const Bounds& bounds,
		coord round_left_top,    coord round_right_top,
		coord round_left_bottom, coord round_right_bottom,
		uint nsegment = 0);

#if 0
	Polygon create_ellipse (
		coord x, coord y, coord width, coord height = 0,
		float angle_from = 0, float angle_to = 360,
		const Point& hole_size = 0, uint nsegment = 0);

	Polygon create_ellipse (
		const Bounds& bounds,
		float angle_from = 0, float angle_to = 360,
		const Point& hole_size = 0, uint nsegment = 0);

	Polygon create_ellipse (
		const Point& center, coord radius,
		float angle_from = 0, float angle_to = 360,
		coord radius_min = 0, uint nsegment = 0);
#endif

}// Rays


#endif//EOH
