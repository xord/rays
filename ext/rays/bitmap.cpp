#include "rays/ruby/bitmap.h"


#include <rucy.h>
#include "rays/exception.h"
#include "rays/ruby/color_space.h"
#include "rays/ruby/color.h"
#include "rays/ruby/font.h"
#include "rays/ruby/texture.h"
#include "defs.h"


using namespace Rucy;

using Rays::uchar;
using Rays::ushort;
using Rays::uint;
using Rays::coord;


RUCY_DEFINE_VALUE_FROM_TO(Rays::Bitmap)

#define THIS  to<Rays::Bitmap*>(self)

#define CHECK RUCY_CHECK_OBJECT(Rays::Bitmap, self)


static
RUCY_DEF_ALLOC(alloc, klass)
{
	return new_type<Rays::Bitmap>(klass);
}
RUCY_END

static
RUCY_DEF1(initialize_copy, obj)
{
	RUCY_CHECK_OBJ(Rays::Bitmap, self);

	*THIS = to<Rays::Bitmap&>(obj).copy();
	return self;
}
RUCY_END

static
RUCY_DEF3(setup, width, height, color_space)
{
	RUCY_CHECK_OBJ(Rays::Bitmap, self);

	Rays::ColorSpace* cs = to<Rays::ColorSpace*>(color_space);
	if (!cs)
		argument_error(__FILE__, __LINE__);

	*THIS = Rays::Bitmap(to<int>(width), to<int>(height), *cs);
	return self;
}
RUCY_END

static
RUCY_DEFN(draw_string)
{
	CHECK;
	check_arg_count(__FILE__, __LINE__, "Bitmap#draw_string", argc, 1, 2, 3, 4);

	const char* str        = to<const char*>(argv[0]);
	coord x                = argc >= 2 ? to<coord>(argv[1]) : 0;
	coord y                = argc >= 3 ? to<coord>(argv[2]) : 0;
	const Rays::Font* font = argc >= 4
		? to<Rays::Font*>(argv[3]) : &Rays::default_font();

	Rays::draw_string(THIS, str, x, y, *font);
	return self;
}
RUCY_END

static
RUCY_DEF0(width)
{
	CHECK;

	return value(THIS->width());
}
RUCY_END

static
RUCY_DEF0(height)
{
	CHECK;

	return value(THIS->height());
}
RUCY_END

static
RUCY_DEF0(color_space)
{
	CHECK;

	return value(THIS->color_space());
}
RUCY_END

static
RUCY_DEF2(at, x, y)
{
	CHECK;

	return value(Rays::Color(THIS->at<void>(x.as_i(), y.as_i()), THIS->color_space()));
}
RUCY_END

static
RUCY_DEF3(set_at, x, y, color)
{
	CHECK;

	Rays::Color* col = to<Rays::Color*>(color);
	if (!col)
		argument_error(__FILE__, __LINE__);

	col->get(THIS->at<void>(x.as_i(), y.as_i()), THIS->color_space());

	return color;
}
RUCY_END

static
RUCY_DEFN(to_texture)
{
	CHECK;

	bool alpha_only = (argc >= 1) ? to<bool>(argv[0]) : false;
	return value(Rays::Texture(*THIS, alpha_only));
}
RUCY_END

static
RUCY_DEF1(save, path)
{
	CHECK;

	Rays::save_bitmap(*THIS, path.c_str());
	return self;
}
RUCY_END


static
RUCY_DEF1(load, path)
{
	return value(Rays::load_bitmap(path.c_str()));
}
RUCY_END


static Class cBitmap;

void
Init_bitmap ()
{
	Module mRays = define_module("Rays");

	cBitmap = mRays.define_class("Bitmap");
	cBitmap.define_alloc_func(alloc);
	cBitmap.define_private_method("initialize_copy", initialize_copy);
	cBitmap.define_private_method("setup", setup);
	cBitmap.define_method("draw_string", draw_string);
	cBitmap.define_method("width", width);
	cBitmap.define_method("height", height);
	cBitmap.define_method("color_space", color_space);
	cBitmap.define_method("[]", at);
	cBitmap.define_method("set_at", set_at);
	cBitmap.define_method("to_texture", to_texture);
	cBitmap.define_method("save", save);
	cBitmap.define_function("load", load);
}


namespace Rays
{


	Class
	bitmap_class ()
	{
		return cBitmap;
	}


}// Rays
