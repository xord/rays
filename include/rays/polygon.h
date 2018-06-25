// -*- c++ -*-
#pragma once
#ifndef __RAYS_POLYGON_H__
#define __RAYS_POLYGON_H__


#include <vector>
#include <xot/pimpl.h>
#include <rays/bounds.h>
#include <rays/polyline.h>


namespace Rays
{


	class Polygon
	{

		public:

			struct Line : public Polyline
			{

				typedef Polyline Super;

				public:

					Line (const Polyline& polyline, bool hole);

					bool hole () const;

					operator bool () const;

					bool operator ! () const;

				private:

					bool hole_;

			};// Line

			typedef std::vector<Line> LineList;

			typedef LineList::const_iterator const_iterator;

			Polygon ();

			Polygon (const Point* points, size_t size, bool loop = true);

			Polygon (const Polyline& polyline);

			~Polygon ();

			size_t size () const;

			bool empty () const;

			const_iterator begin () const;

			const_iterator end () const;

			const Line& operator [] (size_t index) const;

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

			Polygon (Data* data);

	};// Polygon


	Polygon create_line (coord x1, coord y1, coord x2, coord y2);

	Polygon create_line (const Point& p1, const Point& p2);

	Polygon create_line  (const Point* points, size_t size, bool loop = false);


	Polygon create_rect (
		coord x, coord y, coord width, coord height,
		coord round = 0,
		uint nsegment = 0);

	Polygon create_rect (
		coord x, coord y, coord width, coord height,
		coord round_left_top,    coord round_right_top,
		coord round_left_bottom, coord round_right_bottom,
		uint nsegment = 0);

	Polygon create_rect (
		const Bounds& bounds,
		coord round = 0,
		uint nsegment = 0);

	Polygon create_rect (
		const Bounds& bounds,
		coord round_left_top,    coord round_right_top,
		coord round_left_bottom, coord round_right_bottom,
		uint nsegment = 0);


	Polygon create_ellipse (
		coord x, coord y, coord width, coord height = 0,
		const Point& hole_size = 0,
		float angle_from = 0, float angle_to = 360,
		uint nsegment = 0);

	Polygon create_ellipse (
		const Bounds& bounds,
		const Point& hole_size = 0,
		float angle_from = 0, float angle_to = 360,
		uint nsegment = 0);

	Polygon create_ellipse (
		const Point& center, const Point& radius,
		const Point& hole_radius = 0,
		float angle_from = 0, float angle_to = 360,
		uint nsegment = 0);


}// Rays


#endif//EOH
