#include "rays/ruby/polygon.h"


#include <vector>
#include <rucy.h>
#include "rays/ruby/bounds.h"
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

static void
make_line_points (
	std::vector<Rays::Coord3>* points, int argc, const Rucy::Value* argv)
{
	assert(points && argv);

	points->clear();

	if (argv[0].is_num())
	{
		if (argc % 2 != 0)
			argument_error(__FILE__, __LINE__);

		size_t npoints = argc / 2;
		points->reserve(npoints);

		for (size_t i = 0; i < npoints; ++i)
		{
			coord x = to<coord>(argv[i * 2 + 0]);
			coord y = to<coord>(argv[i * 2 + 1]);
			points->emplace_back(Rays::Point(x, y));
		}
	}
	else
	{
		points->reserve(argc);
		for (int i = 0; i < argc; ++i)
			points->emplace_back(to<Rays::Point>(argv[i]));
	}
}

static
RUCY_DEFN(line_strip)
{
	if (argc <= 0)
		argument_error(__FILE__, __LINE__);

	std::vector<Rays::Coord3> points;
	make_line_points(&points, argc, argv);

	return value(
		Rays::create_line((Rays::Point*) &points[0], points.size(), false));
}
RUCY_END

static
RUCY_DEFN(line_loop)
{
	if (argc <= 0)
		argument_error(__FILE__, __LINE__);

	std::vector<Rays::Coord3> points;
	make_line_points(&points, argc, argv);

	return value(create_line((Rays::Point*) &points[0], points.size(), true));
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
	cPolygon.define_method("-", op_sub);
	cPolygon.define_method("&", op_and);
	cPolygon.define_method("|", op_or);
	cPolygon.define_method("^", op_xor);
	cPolygon.define_singleton_method("line_loop",  line_loop);
	cPolygon.define_singleton_method("line_strip", line_strip);
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
