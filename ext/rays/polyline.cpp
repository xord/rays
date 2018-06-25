#include "rays/ruby/polyline.h"


#include <vector>
#include <rucy.h>
#include "rays/ruby/point.h"
#include "defs.h"


using namespace Rucy;

using Rays::coord;


RUCY_DEFINE_VALUE_OR_ARRAY_FROM_TO(Rays::Polyline)

#define THIS  to<Rays::Polyline*>(self)

#define CHECK RUCY_CHECK_OBJECT(Rays::Polyline, self)


static
RUCY_DEF_ALLOC(alloc, klass)
{
	return new_type<Rays::Polyline>(klass);
}
RUCY_END

static
RUCY_DEF2(set_points, points, loop)
{
	CHECK;

	std::vector<Rays::Point> array;
	if (!points.empty() && points[0].is_num())
	{
		if (points.size() % 2 != 0)
			argument_error(__FILE__, __LINE__);

		size_t size = points.size();
		array.reserve(size / 2);
		for (size_t i = 0; i < size; i += 2)
		{
			coord x = to<coord>(points[i + 0]);
			coord y = to<coord>(points[i + 1]);
			array.emplace_back(Rays::Point(x, y));
		}
	}
	else
	{
		size_t size = points.size();
		array.reserve(size);
		for (size_t i = 0; i < size; ++i)
			array.emplace_back(to<Rays::Point>(points[i]));
	}

	*THIS = Rays::Polyline(&array[0], array.size(), loop);
}
RUCY_END

static
RUCY_DEF0(loop)
{
	CHECK;
	return value(THIS->loop());
}
RUCY_END

static
RUCY_DEF0(size)
{
	CHECK;
	return value(THIS->size());
}
RUCY_END

static
RUCY_DEF0(empty)
{
	CHECK;
	return value(THIS->empty());
}
RUCY_END

static
RUCY_DEF1(at, index)
{
	CHECK;

	int size = (int) THIS->size();
	int i    = to<int>(index);
	if (i < 0) i += size;

	if (i < 0 || size <= i)
		index_error(__FILE__, __LINE__);

	return value((*THIS)[i]);
}
RUCY_END

static
RUCY_DEF0(each)
{
	CHECK;

	Value ret;
	for (const auto& point : *THIS)
		ret = rb_yield(value(point));
	return ret;
}
RUCY_END


static Class cPolyline;

void
Init_polyline ()
{
	Module mRays = define_module("Rays");

	cPolyline = mRays.define_class("Polyline");
	cPolyline.define_alloc_func(alloc);
	cPolyline.define_private_method("set_points", set_points);
	cPolyline.define_method("loop?",  loop);
	cPolyline.define_method("size",   size);
	cPolyline.define_method("empty?", empty);
	cPolyline.define_method("[]",     at);
	cPolyline.define_method("each",   each);
}


namespace Rays
{


	Class
	polyline_class ()
	{
		return cPolyline;
	}


}// Rays
