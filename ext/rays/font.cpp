#include "rays/ruby/font.h"


#include <rucy.h>
#include "defs.h"


using namespace Rucy;

using Rays::coord;


RUCY_DEFINE_VALUE_FROM_TO(Rays::Font)

#define THIS  to<Rays::Font*>(self)

#define CHECK RUCY_CHECK_OBJECT(Rays::Font, self)


static
RUCY_DEF_ALLOC(alloc, klass)
{
	return new_type<Rays::Font>(klass);
}
RUCY_END

static
RUCY_DEFN(initialize)
{
	RUCY_CHECK_OBJ(Rays::Font, self);
	check_arg_count(__FILE__, __LINE__, "Font#initialize", argc, 0, 1, 2);

	const char* name = (argc >= 1) ? argv[0].c_str()    : NULL;
	float size       = (argc >= 2) ? to<float>(argv[1]) : 0;
	*THIS = Rays::Font(name, size);

	return self;
}
RUCY_END

static
RUCY_DEF1(initialize_copy, obj)
{
	RUCY_CHECK_OBJ(Rays::Font, self);

	*THIS = to<Rays::Font&>(obj).copy();
	return self;
}
RUCY_END

static
RUCY_DEF0(name)
{
	CHECK;
	return value(THIS->name().c_str());
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
RUCY_DEF1(width, str)
{
	CHECK;
	return value(THIS->get_width(str.c_str()));
}
RUCY_END

static
RUCY_DEF0(height)
{
	CHECK;
	return value(THIS->get_height());
}
RUCY_END


static Class cFont;

void
Init_font ()
{
	Module mRays = define_module("Rays");

	cFont = mRays.define_class("Font");
	cFont.define_alloc_func(alloc);
	cFont.define_private_method("initialize", initialize);
	cFont.define_private_method("initialize_copy", initialize_copy);
	cFont.define_method("name", name);
	cFont.define_method("size", size);
	cFont.define_method("width", width);
	cFont.define_method("height", height);
}


namespace Rays
{


	Class
	font_class ()
	{
		return cFont;
	}


}// Rays
