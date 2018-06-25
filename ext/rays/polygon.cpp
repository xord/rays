#include "rays/ruby/polygon.h"


#include <vector>
#include "rays/ruby/bounds.h"
#include "rays/ruby/polyline.h"
#include "defs.h"


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
RUCY_DEF2(set_points, args, loop)
{
	CHECK;

	std::vector<Rays::Point> points;
	bool line_loop;
	get_line_args(&points, &line_loop, args, loop);

	*THIS = Rays::Polygon(&points[0], points.size(), line_loop);
}
RUCY_END

static
RUCY_DEF1(set_polyline, polyline)
{
	CHECK;
	*THIS = Rays::Polygon(to<Rays::Polyline&>(polyline));
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
	for (const auto& line : *THIS)
		ret = rb_yield(value(line));
	return ret;
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
RUCY_DEF7(create_rect,
	args, round, lefttop, righttop, leftbottom, rightbottom, nsegment)
{
	coord x, y, w, h, lt, rt, lb, rb;
	uint nseg;
	get_rect_args(
		&x, &y, &w, &h, &lt, &rt, &lb, &rb, &nseg,
		args, round, lefttop, righttop, leftbottom, rightbottom, nsegment);

	return value(Rays::create_rect(x, y, w, h, lt, rt, lb, rb, nseg));
}
RUCY_END

static
RUCY_DEF7(create_ellipse,
	args, center, radius, hole, angle_from, angle_to, nsegment)
{
	coord x, y, w, h;
	Rays::Point hole_size;
	float from, to_;
	uint nseg;
	get_ellipse_args(
		&x, &y, &w, &h, &hole_size, &from, &to_, &nseg,
		args, center, radius, hole, angle_from, angle_to, nsegment);

	return value(Rays::create_ellipse(x, y, w, h, hole_size, from, to_, nseg));
}
RUCY_END


static Class cPolygon;

void
Init_polygon ()
{
	Module mRays = define_module("Rays");

	cPolygon = mRays.define_class("Polygon");
	cPolygon.define_alloc_func(alloc);
	cPolygon.define_private_method("set_points",   set_points);
	cPolygon.define_private_method("set_polyline", set_polyline);
	cPolygon.define_method("size",   size);
	cPolygon.define_method("empty?", empty);
	cPolygon.define_method("[]",     at);
	cPolygon.define_method("each",   each);
	cPolygon.define_method("-", op_sub);
	cPolygon.define_method("&", op_and);
	cPolygon.define_method("|", op_or);
	cPolygon.define_method("^", op_xor);
	cPolygon.define_singleton_method("create_rect",    create_rect);
	cPolygon.define_singleton_method("create_ellipse", create_ellipse);
}


namespace Rays
{


	Class
	polygon_class ()
	{
		return cPolygon;
	}


}// Rays
