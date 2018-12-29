#include "rays/ruby/polyline.h"


#include <vector>
#include "rays/ruby/point.h"
#include "rays/ruby/bounds.h"
#include "rays/ruby/polygon.h"
#include "defs.h"


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
RUCY_DEF2(set_points, args, loop)
{
	CHECK;

	std::vector<Rays::Point> points;
	bool line_loop;
	get_line_args(&points, &line_loop, args, loop);

	*THIS = Rays::Polyline(&points[0], points.size(), line_loop);
}
RUCY_END

static
RUCY_DEF1(expand, width)
{
	CHECK;

	Rays::Polygon polygon;
	THIS->expand(&polygon, to<coord>(width));
	return value(polygon);
}
RUCY_END

static
RUCY_DEF0(bounds)
{
	CHECK;
	return value(THIS->bounds());
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
	cPolyline.define_method("expand", expand);
	cPolyline.define_method("bounds", bounds);
	cPolyline.define_method("loop?", loop);
	cPolyline.define_method("size", size);
	cPolyline.define_method("empty?", empty);
	cPolyline.define_method("[]", at);
	cPolyline.define_method("each", each);
}


namespace Rays
{


	Class
	polyline_class ()
	{
		return cPolyline;
	}


}// Rays
