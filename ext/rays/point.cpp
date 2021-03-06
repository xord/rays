#include "rays/ruby/point.h"


#include <rucy.h>
#include "defs.h"


using namespace Rucy;

using Rays::coord;


RUCY_DEFINE_VALUE_FROM_TO(Rays::Point)

#define THIS  to<Rays::Point*>(self)

#define CHECK RUCY_CHECK_OBJ(Rays::Point, self)


static
RUCY_DEF_ALLOC(alloc, klass)
{
	return new_type<Rays::Point>(klass);
}
RUCY_END

static
RUCY_DEFN(initialize)
{
	CHECK;
	check_arg_count(__FILE__, __LINE__, "Point#initialize", argc, 0, 1, 2, 3);

	switch (argc)
	{
		case 1:
			*THIS = Rays::Point(to<coord>(argv[0]));
			break;

		case 2:
			*THIS = Rays::Point(to<coord>(argv[0]), to<coord>(argv[1]));
			break;

		case 3:
			*THIS = Rays::Point(
				to<coord>(argv[0]), to<coord>(argv[1]), to<coord>(argv[2]));
			break;
	}

	return self;
}
RUCY_END

static
RUCY_DEF1(initialize_copy, obj)
{
	CHECK;
	*THIS = to<Rays::Point&>(obj);
	return self;
}
RUCY_END

static
RUCY_DEFN(move_to)
{
	CHECK;
	check_arg_count(__FILE__, __LINE__, "Point#move_to", argc, 1, 2, 3);

	if (argv[0].is_kind_of(Rays::point_class()))
		THIS->move_to(to<Rays::Point&>(argv[0]));
	else
	{
		const Rays::Point& p = *THIS;
		coord x = (argc >= 1 && argv[0]) ? to<coord>(argv[0]) : p.x;
		coord y = (argc >= 2 && argv[1]) ? to<coord>(argv[1]) : p.y;
		coord z = (argc >= 3 && argv[2]) ? to<coord>(argv[2]) : p.z;
		THIS->move_to(x, y, z);
	}

	return self;
}
RUCY_END

static
RUCY_DEFN(move_by)
{
	CHECK;
	check_arg_count(__FILE__, __LINE__, "Point#move_by", argc, 1, 2, 3);

	if (argv[0].is_kind_of(Rays::point_class()))
		THIS->move_by(to<Rays::Point&>(argv[0]));
	else
	{
		coord x = (argc >= 1 && argv[0]) ? to<coord>(argv[0]) : 0;
		coord y = (argc >= 2 && argv[1]) ? to<coord>(argv[1]) : 0;
		coord z = (argc >= 3 && argv[2]) ? to<coord>(argv[2]) : 0;
		THIS->move_by(x, y, z);
	}

	return self;
}
RUCY_END

static
RUCY_DEF0(length)
{
	CHECK;
	return value(THIS->length());
}
RUCY_END

static
RUCY_DEF0(normalize)
{
	CHECK;
	THIS->normalize();
	return self;
}
RUCY_END

static
RUCY_DEF0(normal)
{
	CHECK;
	return value(THIS->normal());
}
RUCY_END

static
RUCY_DEF1(set_x, x)
{
	CHECK;
	return value(THIS->x = to<coord>(x));
}
RUCY_END

static
RUCY_DEF0(get_x)
{
	CHECK;
	return value(THIS->x);
}
RUCY_END

static
RUCY_DEF1(set_y, y)
{
	CHECK;
	return value(THIS->y = to<coord>(y));
}
RUCY_END

static
RUCY_DEF0(get_y)
{
	CHECK;
	return value(THIS->y);
}
RUCY_END

static
RUCY_DEF1(set_z, z)
{
	CHECK;
	return value(THIS->z = to<coord>(z));
}
RUCY_END

static
RUCY_DEF0(get_z)
{
	CHECK;
	return value(THIS->z);
}
RUCY_END

static
RUCY_DEF1(add, point)
{
	CHECK;

	Rays::Point p = *THIS;
	p += to<Rays::Point&>(point);
	return value(p);
}
RUCY_END

static
RUCY_DEF1(sub, point)
{
	CHECK;

	Rays::Point p = *THIS;
	p -= to<Rays::Point&>(point);
	return value(p);
}
RUCY_END

static
RUCY_DEF1(mult, point)
{
	CHECK;

	Rays::Point p = *THIS;
	p *= to<Rays::Point&>(point);
	return value(p);
}
RUCY_END

static
RUCY_DEF1(div, point)
{
	CHECK;

	Rays::Point p = *THIS;
	p /= to<Rays::Point&>(point);
	return value(p);
}
RUCY_END

static
RUCY_DEF1(array_get, index)
{
	CHECK;

	int i = index.as_i();
	if (i < 0 || 2 < i)
		index_error(__FILE__, __LINE__);

	return value((*THIS)[i]);
}
RUCY_END

static
RUCY_DEF2(array_set, index, value)
{
	CHECK;

	int i = index.as_i();
	if (i < 0 || 2 < i)
		index_error(__FILE__, __LINE__);

	(*THIS)[i] = to<coord>(value);
	return value;
}
RUCY_END

static
RUCY_DEF0(inspect)
{
	CHECK;
	return value(Xot::stringf("#<Rays::Point %s>", THIS->inspect().c_str()));
}
RUCY_END


static Class cPoint;

void
Init_point ()
{
	Module mRays = define_module("Rays");

	cPoint = mRays.define_class("Point");
	cPoint.define_alloc_func(alloc);
	cPoint.define_private_method("initialize", initialize);
	cPoint.define_private_method("initialize_copy", initialize_copy);
	cPoint.define_method("move_to!", move_to);
	cPoint.define_method("move_by!", move_by);
	cPoint.define_method("length", length);
	cPoint.define_method("normalize", normalize);
	cPoint.define_method("normal",    normal);
	cPoint.define_method("x=", set_x);
	cPoint.define_method("x", get_x);
	cPoint.define_method("y=", set_y);
	cPoint.define_method("y", get_y);
	cPoint.define_method("z=", set_z);
	cPoint.define_method("z", get_z);
	cPoint.define_method("op_add", add);
	cPoint.define_method("op_sub", sub);
	cPoint.define_method("op_mult", mult);
	cPoint.define_method("op_div", div);
	cPoint.define_method("[]",  array_get);
	cPoint.define_method("[]=", array_set);
	cPoint.define_method("inspect", inspect);
}


namespace Rucy
{


	template <> Rays::Point
	value_to<Rays::Point> (Value value, bool convert)
	{
		if (convert)
		{
			size_t argc = 0;
			Value* argv = NULL;
			if (value.is_array())
			{
				argc = value.size();
				argv = value.as_array();
			}
			else
			{
				argc = 1;
				argv = &value;
			}

			if (argc < 1)
				Rucy::argument_error(__FILE__, __LINE__);

			if (argv[0].is_kind_of(Rays::point_class()))
				value = argv[0];
			else if (argv[0].is_i() || argv[0].is_f())
			{
				switch (argc)
				{
					#define V(i) argv[i].as_f(true)
					case 1: return Rays::Point(V(0));
					case 2: return Rays::Point(V(0), V(1));
					case 3: return Rays::Point(V(0), V(1), V(2));
					#undef V
					default: Rucy::argument_error(__FILE__, __LINE__);
				}
			}
		}

		return value_to<Rays::Point&>(value, convert);
	}


}// Rucy


namespace Rays
{


	Class
	point_class ()
	{
		return cPoint;
	}


}// Rays
