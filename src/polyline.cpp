#include "polyline.h"


#include <float.h>
#include <assert.h>
#include "rays/exception.h"
#include "rays/debug.h"


using namespace ClipperLib;


namespace Rays
{


	static const double CLIPPER_SCALE = 1000;


	static inline cInt
	to_clipper (coord value)
	{
		return (cInt) (value * CLIPPER_SCALE);
	}

	static inline coord
	from_clipper (cInt value)
	{
		double v = value / CLIPPER_SCALE;
		if (v <= FLT_MIN || FLT_MAX <= v)
			argument_error(__FILE__, __LINE__);

		return (coord) v;
	}

	static inline IntPoint
	to_clipper (const Point& point)
	{
		return IntPoint(
			to_clipper(point.x),
			to_clipper(point.y));
	}

	static inline Point
	from_clipper (const IntPoint& point)
	{
		return Point(
			from_clipper(point.X),
			from_clipper(point.Y));
	}


	struct Polyline::Data
	{

		PointList points;

		template <typename I, typename FUN>
		void reset (I begin, I end, bool loop, FUN to_point_fun)
		{
			size_t size = end - begin;
			if (size < 2 || (size == 2 && loop))
				argument_error(__FILE__, __LINE__);

			points.clear();

			bool append_to_close = loop && *begin != *(end - 1);
			points.reserve(size + append_to_close ? 1 : 0);

			for (auto it = begin; it != end; ++it)
				points.emplace_back(to_point_fun(*it));

			if (append_to_close)
				points.emplace_back(points.front());
		}

	};// Polyline::Data


	void
	Polyline_create (
		Polyline* polyline, const Path& path, bool loop, bool reverse)
	{
		assert(polyline);

		auto to_point = [](const IntPoint& point) {return from_clipper(point);};
		if (reverse)
			polyline->self->reset(path.rbegin(), path.rend(), loop, to_point);
		else
			polyline->self->reset(path.begin(), path.end(), loop, to_point);
	}

	template <typename I>
	static void
	reset_path (Path* path, I begin, I end)
	{
		path->clear();
		for (auto it = begin; it != end; ++it)
			path->emplace_back(to_clipper(*it));
	}

	void
	Polyline_get_path (Path* path, const Polyline& polyline, bool reverse)
	{
		assert(path);

		const auto& points = polyline.self->points;
		if (reverse)
			reset_path(path, points.rbegin(), points.rend());
		else
			reset_path(path, points.begin(), points.end());
	}


	Polyline::Polyline ()
	{
	}

	Polyline::Polyline (const Point* points, size_t size, bool loop)
	{
		self->reset(points, points + size, loop, [](const Point& p) {return p;});
	}

	Polyline::~Polyline ()
	{
	}

	Polyline
	Polyline::dup () const
	{
		Polyline p;
		p.self->points = self->points;
		return p;
	}

	bool
	Polyline::loop () const
	{
		const PointList& p = self->points;
		return p.size() >= 3 && p.front() == p.back();
	}

	size_t
	Polyline::size () const
	{
		return self->points.size();
	}

	bool
	Polyline::empty () const
	{
		return size() <= 0;
	}

	Polyline::const_iterator
	Polyline::begin () const
	{
		return self->points.begin();
	}

	Polyline::const_iterator
	Polyline::end () const
	{
		return self->points.end();
	}

	const Point&
	Polyline::operator [] (size_t index) const
	{
		return self->points[index];
	}

	Polyline::operator bool () const
	{
		return !empty();
	}

	bool
	Polyline::operator ! () const
	{
		return !operator bool();
	}


}// Rays
