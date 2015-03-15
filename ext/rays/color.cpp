#include "rays/ruby/color.h"


#include <rucy.h>
#include "defs.h"


using namespace Rucy;


RUCY_DEFINE_VALUE_FROM_TO(Rays::Color)

#define THIS  to<Rays::Color*>(self)

#define CHECK RUCY_CHECK_OBJ(Rays::Color, self)


static
RUCY_DEF_ALLOC(alloc, klass)
{
	return new_type<Rays::Color>(klass);
}
RUCY_END

static
RUCY_DEFN(setup)
{
	CHECK;
	check_arg_count(__FILE__, __LINE__, "Color#initialize", argc, 0, 1, 2, 3, 4);

	if (argc == 0) return self;

	switch (argc)
	{
		case 1:
			*THIS = Rays::Color(to<float>(argv[0]));
			break;

		case 2:
			*THIS = Rays::Color(to<float>(argv[0]), to<float>(argv[1]));
			break;

		case 3:
			*THIS = Rays::Color(
				to<float>(argv[0]), to<float>(argv[1]), to<float>(argv[2]));
			break;

		case 4:
			*THIS = Rays::Color(
				to<float>(argv[0]), to<float>(argv[1]),
				to<float>(argv[2]), to<float>(argv[3]));
			break;
	}

	return self;
}
RUCY_END

static
RUCY_DEF1(initialize_copy, obj)
{
	CHECK;
	*THIS = to<Rays::Color&>(obj);
	return self;
}
RUCY_END

static
RUCY_DEF1(set_red, red)
{
	CHECK;

	return value(THIS->red = to<float>(red));
}
RUCY_END

static
RUCY_DEF0(get_red)
{
	CHECK;

	return value(THIS->red);
}
RUCY_END

static
RUCY_DEF1(set_green, green)
{
	CHECK;

	return value(THIS->green = to<float>(green));
}
RUCY_END

static
RUCY_DEF0(get_green)
{
	CHECK;

	return value(THIS->green);
}
RUCY_END

static
RUCY_DEF1(set_blue, blue)
{
	CHECK;

	return value(THIS->blue = to<float>(blue));
}
RUCY_END

static
RUCY_DEF0(get_blue)
{
	CHECK;

	return value(THIS->blue);
}
RUCY_END

static
RUCY_DEF1(set_alpha, alpha)
{
	CHECK;

	return value(THIS->alpha = to<float>(alpha));
}
RUCY_END

static
RUCY_DEF0(get_alpha)
{
	CHECK;

	return value(THIS->alpha);
}
RUCY_END


static Class cColor;

void
Init_color ()
{
	Module mRays = define_module("Rays");

	cColor = mRays.define_class("Color");
	cColor.define_alloc_func(alloc);
	cColor.define_private_method("setup", setup);
	cColor.define_private_method("initialize_copy", initialize_copy);
	cColor.define_method("red=", set_red);
	cColor.define_method("red", get_red);
	cColor.define_method("green=", set_green);
	cColor.define_method("green", get_green);
	cColor.define_method("blue=", set_blue);
	cColor.define_method("blue", get_blue);
	cColor.define_method("alpha=", set_alpha);
	cColor.define_method("alpha", get_alpha);
}


namespace Rucy
{


	template <> Rays::Color
	value_to<Rays::Color> (Value value, bool convert)
	{
		if (convert)
		{
			if (value.is_i() || value.is_f())
				return Rays::Color(value.as_f(true));
			else if (value.is_array())
			{
				int size = value.size();
				if (size <= 0 || 4 < size)
					Rucy::argument_error(__FILE__, __LINE__);

				Value* a = value.as_array();
				switch (size)
				{
					case 1: return Rays::Color(a[0].as_f(true));
					case 2: return Rays::Color(a[0].as_f(true), a[1].as_f(true));
					case 3: return Rays::Color(a[0].as_f(true), a[1].as_f(true), a[2].as_f(true));
					case 4: return Rays::Color(a[0].as_f(true), a[1].as_f(true), a[2].as_f(true), a[3].as_f(true));
				}
			}
		}

		return value_to<Rays::Color&>(value, convert);
	}


}// Rucy


namespace Rays
{


	Class
	color_class ()
	{
		return cColor;
	}


}// Rays
