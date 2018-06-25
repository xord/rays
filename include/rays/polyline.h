// -*- c++ -*-
#pragma once
#ifndef __RAYS_POLYLINE_H__
#define __RAYS_POLYLINE_H__


#include <vector>
#include <xot/pimpl.h>
#include <rays/point.h>


namespace Rays
{


	class Polyline
	{

		public:

			typedef std::vector<Point> PointList;

			typedef PointList::const_iterator const_iterator;

			Polyline ();

			Polyline (const Point* points, size_t size, bool loop = true);

			~Polyline ();

			Polyline dup () const;

			bool loop () const;

			const_iterator begin () const;

			const_iterator end () const;

			size_t size () const;

			bool empty () const;

			const Point& operator [] (size_t index) const;

			operator bool () const;

			bool operator ! () const;

			struct Data;

			Xot::PSharedImpl<Data> self;

	};// Polyline


}// Rays


#endif//EOH
