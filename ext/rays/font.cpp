#include "rays/ruby/font.h"


#include <rucy.h>
#include "defs.h"


using namespace Rucy;

using Rays::coord;


RUCY_DEFINE_VALUE_OR_ARRAY_FROM_TO(Rays::Font)

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

	*THIS = to<Rays::Font>(argc, argv);
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
	cFont.define_private_method("initialize",      initialize);
	cFont.define_private_method("initialize_copy", initialize_copy);
	cFont.define_method("name", name);
	cFont.define_method("size", size);
	cFont.define_method("width", width);
	cFont.define_method("height", height);
}


namespace Rucy
{


	template <> Rays::Font
	value_to<Rays::Font> (int argc, const Value* argv, bool convert)
	{
		if (argc == 1 && argv->is_array())
		{
			argc = argv->size();
			argv = argv->as_array();
		}

		assert(argc == 0 || (argc > 0 && argv));

		if (convert)
		{
			if (argc == 0)
				return Rays::default_font();

			coord size = argc >= 2 ? to<coord>(argv[1]) : 0;
			if (argv->is_nil())
				return Rays::Font(NULL, size);
			else if (argv->is_s() || argv->is_sym())
				return Rays::Font(argv[0].c_str(), size);
		}

		if (argc != 1)
			argument_error(__FILE__, __LINE__);

		return value_to<Rays::Font&>(*argv, convert);
	}


}// Rucy


namespace Rays
{


	Class
	font_class ()
	{
		return cFont;
	}


}// Rays
