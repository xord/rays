#include "rays/ruby/polygon.h"


#include <vector>
#include <rucy.h>
#include "rays/ruby/bounds.h"
#include "rays/ruby/polyline.h"
#include "defs.h"


using namespace Rucy;

using Rays::coord;


RUCY_DEFINE_VALUE_OR_ARRAY_FROM_TO(Rays::Polygon)

#define THIS  to<Rays::Polygon*>(self)

#define CHECK RUCY_CHECK_OBJECT(Rays::Polygon, self)


static
RUCY_DEF_ALLOC(alloc, klass)
{
	return new_type<Rays::Polygon>(klass);
}
RUCY_END

static
RUCY_DEF1(initialize_copy, obj)
{
	CHECK;
	*THIS = to<Rays::Polygon&>(obj).dup();
	return self;
}
RUCY_END

static
RUCY_DEF3(add_points, points, loop, hole)
{
	CHECK;

	if (points.size() <= 0) return nil();

	std::vector<Rays::Point> array;
	if (points[0].is_num())
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

	THIS->add(&array[0], array.size(), loop, hole);
}
RUCY_END

static
RUCY_DEF2(add_polyline, polyline, hole)
{
	CHECK;
	THIS->add(to<Rays::Polyline&>(polyline), hole);
}
RUCY_END

static
RUCY_DEF0(clear)
{
	CHECK;
	THIS->clear();
}
RUCY_END

static
RUCY_DEF0(each)
{
	CHECK;

	Value ret;
	for (const auto& polyline : *THIS)
		ret = rb_yield(value(polyline));
	return ret;
}
RUCY_END

static
RUCY_DEF0(size)
{
	CHECK;
	return THIS->size();
}
RUCY_END

static
RUCY_DEF0(empty)
{
	CHECK;
	return THIS->empty();
}
RUCY_END

static
RUCY_DEF1(at, index)
{
	CHECK;
	return value((*THIS)[index]);
}
RUCY_END

static
RUCY_DEF1(op_sub, obj)
{
	CHECK;
	return value(*THIS - to<Rays::Polygon&>(obj));
}
RUCY_END

static
RUCY_DEF1(op_and, obj)
{
	CHECK;
	return value(*THIS & to<Rays::Polygon&>(obj));
}
RUCY_END

static
RUCY_DEF1(op_or, obj)
{
	CHECK;
	return value(*THIS | to<Rays::Polygon&>(obj));
}
RUCY_END

static
RUCY_DEF1(op_xor, obj)
{
	CHECK;
	return value(*THIS ^ to<Rays::Polygon&>(obj));
}
RUCY_END

static
RUCY_DEFN(rect)
{
	check_arg_count(
		__FILE__, __LINE__, "Polygon.rect", argc, 1, 2, 4, 5, 6, 8, 9);

	if (argv[0].is_kind_of(Rays::bounds_class()))
	{
		Rays::Bounds& b = to<Rays::Bounds&>(argv[0]);
		coord lt        = argc >= 2 ? to<coord>(argv[1]) : 0;
		coord rt        = argc >= 3 ? to<coord>(argv[2]) : lt;
		coord lb        = argc >= 4 ? to<coord>(argv[3]) : lt;
		coord rb        = argc >= 5 ? to<coord>(argv[4]) : lt;
		uint nsegment   = argc >= 6 ? to<uint>(argv[5])  : 0;
		return value(Rays::create_rect(b, lt, rt, lb, rb, nsegment));
	}
	else
	{
		coord x       = to<coord>(argv[0]);
		coord y       = to<coord>(argv[1]);
		coord w       = to<coord>(argv[2]);
		coord h       = to<coord>(argv[3]);
		coord lt      = argc >= 5 ? to<coord>(argv[4]) : 0;
		coord rt      = argc >= 6 ? to<coord>(argv[5]) : lt;
		coord lb      = argc >= 7 ? to<coord>(argv[6]) : lt;
		coord rb      = argc >= 8 ? to<coord>(argv[7]) : lt;
		uint nsegment = argc >= 9 ? to<uint>(argv[8])  : 0;
		return value(Rays::create_rect(x, y, w, h, lt, rt, lb, rb, nsegment));
	}
}
RUCY_END


static Class cPolygon;

void
Init_polygon ()
{
	Module mRays = define_module("Rays");

	cPolygon = mRays.define_class("Polygon");
	cPolygon.define_alloc_func(alloc);
	cPolygon.define_private_method("initialize_copy", initialize_copy);
	cPolygon.define_method("add_points",   add_points);
	cPolygon.define_method("add_polyline", add_polyline);
	cPolygon.define_method("clear",  clear);
	cPolygon.define_method("each",   each);
	cPolygon.define_method("size",   size);
	cPolygon.define_method("empty?", empty);
	cPolygon.define_method("at",     at);
	cPolygon.define_method("-", op_sub);
	cPolygon.define_method("&", op_and);
	cPolygon.define_method("|", op_or);
	cPolygon.define_method("^", op_xor);
	cPolygon.define_singleton_method("rect", rect);
}


namespace Rays
{


	Class
	polygon_class ()
	{
		return cPolygon;
	}


}// Rays
