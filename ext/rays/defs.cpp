#include "defs.h"


#include <assert.h>
#include "rays/ruby/bounds.h"
#include "rays/ruby/point.h"


void
get_line_args (
	std::vector<Rays::Point>* points, bool* line_loop,
	Value args, Value loop)
{
	assert(points && line_loop);

	points->clear();
	*line_loop = true;

	if (args.empty())
		return;

	size_t argc = args.size();
	if (args[0].is_num())
	{
		if (argc % 2 != 0)
			argument_error(__FILE__, __LINE__);

		points->reserve(argc / 2);
		for (size_t i = 0; i < argc; i += 2)
		{
			coord x = to<coord>(args[i + 0]);
			coord y = to<coord>(args[i + 1]);
			points->emplace_back(Rays::Point(x, y));
		}
	}
	else
	{
		points->reserve(argc);
		for (size_t i = 0; i < argc; ++i)
			points->emplace_back(to<Rays::Point>(args[i]));
	}

	*line_loop = loop;
}

static uint
get_nsegment (Value nsegment)
{
	int value = nsegment ? to<int>(nsegment) : 0;
	if (value < 0) value = 0;
	return (uint) value;
}

void get_rect_args (
	coord* x, coord* y, coord* w, coord* h,
	coord* lt, coord* rt, coord* lb, coord* rb,
	uint* nseg,
	Value args, Value round,
	Value lefttop, Value righttop, Value leftbottom, Value rightbottom,
	Value nsegment)
{
	assert(x && y && w && h && lt && rt && lb && rb && nseg);

	if (args.empty())
		argument_error(__FILE__, __LINE__);

	size_t argc = args.size();

	if (args[0].is_kind_of(Rays::bounds_class()))
	{
		Rays::Bounds& b = to<Rays::Bounds&>(args[0]);
		*x  = b.x;
		*y  = b.y;
		*w  = b.w;
		*h  = b.h;
		*lt = argc >= 2 ? to<coord>(args[1]) : 0;
		*rt = argc >= 3 ? to<coord>(args[2]) : *lt;
		*lb = argc >= 4 ? to<coord>(args[3]) : *lt;
		*rb = argc >= 5 ? to<coord>(args[4]) : *lt;
	}
	else if (args[0].is_kind_of(Rays::point_class()))
	{
		if (argc < 2)
			argument_error(__FILE__, __LINE__);

		Rays::Point& p = to<Rays::Point&>(args[0]);
		*x  = p.x;
		*y  = p.y;
		*w  =             to<coord>(args[1]);
		*h  = argc >= 3 ? to<coord>(args[2]) : *w;
		*lt = argc >= 4 ? to<coord>(args[3]) : 0;
		*rt = argc >= 5 ? to<coord>(args[4]) : *lt;
		*lb = argc >= 6 ? to<coord>(args[5]) : *lt;
		*rb = argc >= 7 ? to<coord>(args[6]) : *lt;
	}
	else if (argc <= 2)
	{
		*x = *y = *lt = *rt = *lb = *rb = 0;
		*w =             to<coord>(args[0]);
		*h = argc >= 2 ? to<coord>(args[1]) : *w;
	}
	else
	{
		*x  =             to<coord>(args[0]);
		*y  =             to<coord>(args[1]);
		*w  =             to<coord>(args[2]);
		*h  = argc >= 4 ? to<coord>(args[3]) : *w;
		*lt = argc >= 5 ? to<coord>(args[4]) : 0;
		*rt = argc >= 6 ? to<coord>(args[5]) : *lt;
		*lb = argc >= 7 ? to<coord>(args[6]) : *lt;
		*rb = argc >= 8 ? to<coord>(args[7]) : *lt;
	}

	if (! lefttop)     lefttop    = round;
	if (!righttop)    righttop    = round;
	if (! leftbottom)  leftbottom = round;
	if (!rightbottom) rightbottom = round;

	if ( lefttop)    *lt = to<coord>( lefttop);
	if (righttop)    *rt = to<coord>(righttop);
	if ( leftbottom) *lb = to<coord>( leftbottom);
	if (rightbottom) *rb = to<coord>(rightbottom);

	*nseg = get_nsegment(nsegment);
}

void get_ellipse_args (
	coord* x, coord* y, coord* w, coord* h,
	Rays::Point* hole_size, float* from, float* to_,
	uint* nseg,
	Value args, Value center, Value radius,
	Value hole, Value angle_from, Value angle_to,
	Value nsegment)
{
	assert(x && y && w && h && hole_size && from && to_ && nseg);

	if (args.empty())
		argument_error(__FILE__, __LINE__);

	size_t argc = args.size();

	if (args[0].is_kind_of(Rays::bounds_class()))
	{
		const Rays::Bounds& b = to<Rays::Bounds&>(args[0]);
		*x = b.x;
		*y = b.y;
		*w = b.w;
		*h = b.h;
	}
	else if (args[0].is_kind_of(Rays::point_class()))
	{
		if (argc < 2)
			argument_error(__FILE__, __LINE__);

		const Rays::Point& p = to<Rays::Point&>(args[0]);
		*x = p.x;
		*y = p.y;
		*w =             to<coord>(args[1]);
		*h = argc >= 3 ? to<coord>(args[2]) : *w;
	}
	else if (argc <= 2)
	{
		*x = *y = 0;
		*w =             to<coord>(args[0]);
		*h = argc >= 2 ? to<coord>(args[1]) : *w;
	}
	else
	{
		*x =             to<coord>(args[0]);
		*y =             to<coord>(args[1]);
		*w =             to<coord>(args[2]);
		*h = argc >= 4 ? to<coord>(args[3]) : *w;
	}

	if (center)
	{
		Rays::Point p = to<Rays::Point>(center);
		*x = p.x;
		*y = p.y;
	}

	if (radius)
	{
		Rays::Point p = to<Rays::Point>(radius);
		*w = p.x * 2;
		*h = p.y * 2;
	}

	*hole_size = hole       ? to<Rays::Point>(hole) : 0;
	*from      = angle_from ? to<float>(angle_from) : 0;
	*to_       = angle_to   ? to<float>(angle_to)   : 360;
	*nseg      = get_nsegment(nsegment);
}
