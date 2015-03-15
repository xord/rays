#include "rays/ruby/color_space.h"


#include <rucy.h>
#include "defs.h"


using namespace Rucy;


RUCY_DEFINE_VALUE_FROM_TO(Rays::ColorSpace)

#define THIS  to<Rays::ColorSpace*>(self)

#define CHECK RUCY_CHECK_OBJ(Rays::ColorSpace, self)


static
RUCY_DEF_ALLOC(alloc, klass)
{
	return new_type<Rays::ColorSpace>(klass);
}
RUCY_END

static
RUCY_DEF2(setup, type, premultiplied)
{
	CHECK;

	*THIS = Rays::ColorSpace(
		(Rays::ColorSpaceType) to<uint>(type),
		to<bool>(premultiplied));
	return self;
}
RUCY_END

static
RUCY_DEF1(initialize_copy, obj)
{
	CHECK;
	*THIS = to<Rays::ColorSpace&>(obj);
	return self;
}
RUCY_END

static
RUCY_DEF0(type)
{
	CHECK;

	return value(THIS->type());
}
RUCY_END

static
RUCY_DEF0(is_gray)
{
	CHECK;

	return value(THIS->is_gray());
}
RUCY_END

static
RUCY_DEF0(is_rgb)
{
	CHECK;

	return value(THIS->is_rgb());
}
RUCY_END

static
RUCY_DEF0(is_bgr)
{
	CHECK;

	return value(THIS->is_bgr());
}
RUCY_END

static
RUCY_DEF0(is_float)
{
	CHECK;

	return value(THIS->is_float());
}
RUCY_END

static
RUCY_DEF0(has_alpha)
{
	CHECK;

	return value(THIS->has_alpha());
}
RUCY_END

static
RUCY_DEF0(has_skip)
{
	CHECK;

	return value(THIS->has_skip());
}
RUCY_END

static
RUCY_DEF0(is_premult)
{
	CHECK;

	return value(THIS->is_premult());
}
RUCY_END


static Class cColorSpace;

void
Init_color_space ()
{
	Module mRays = define_module("Rays");

	cColorSpace = mRays.define_class("ColorSpace");
	cColorSpace.define_alloc_func(alloc);
	cColorSpace.define_private_method("initialize_copy", initialize_copy);
	cColorSpace.define_private_method("setup", setup);
	cColorSpace.define_method("type",     type);
	cColorSpace.define_method("gray?",    is_gray);
	cColorSpace.define_method("rgb?",     is_rgb);
	cColorSpace.define_method("bgr?",     is_bgr);
	cColorSpace.define_method("float?",   is_float);
	cColorSpace.define_method("alpha?",   has_alpha);
	cColorSpace.define_method("skip?",    has_skip);
	cColorSpace.define_method("premult?", is_premult);

	cColorSpace.define_const("GRAY_8",     Rays::GRAY_8);
	cColorSpace.define_const("GRAY_16",    Rays::GRAY_16);
	cColorSpace.define_const("GRAY_24",    Rays::GRAY_24);
	cColorSpace.define_const("GRAY_32",    Rays::GRAY_32);
	cColorSpace.define_const("GRAY_float", Rays::GRAY_float);
	cColorSpace.define_const("RGB_888",    Rays:: RGB_888);
	cColorSpace.define_const("RGBA_8888",  Rays:: RGBA_8888);
	cColorSpace.define_const("RGBX_8888",  Rays:: RGBX_8888);
	cColorSpace.define_const("ARGB_8888",  Rays::ARGB_8888);
	cColorSpace.define_const("XRGB_8888",  Rays::XRGB_8888);
	cColorSpace.define_const("BGR_888",    Rays:: BGR_888);
	cColorSpace.define_const("BGRA_8888",  Rays:: BGRA_8888);
	cColorSpace.define_const("BGRX_8888",  Rays:: BGRX_8888);
	cColorSpace.define_const("ABGR_8888",  Rays::ABGR_8888);
	cColorSpace.define_const("XBGR_8888",  Rays::XBGR_8888);
	cColorSpace.define_const("RGB_float",  Rays:: RGB_float);
	cColorSpace.define_const("RGBA_float", Rays:: RGBA_float);
	cColorSpace.define_const("ARGB_float", Rays::ARGB_float);
	cColorSpace.define_const("BGR_float",  Rays:: BGR_float);
	cColorSpace.define_const("BGRA_float", Rays:: BGRA_float);
	cColorSpace.define_const("ABGR_float", Rays::ABGR_float);
	cColorSpace.define_const("GRAY",       Rays::GRAY);
	cColorSpace.define_const("RGB",        Rays:: RGB);
	cColorSpace.define_const("BGR",        Rays:: BGR);
	cColorSpace.define_const("RGBA",       Rays:: RGBA);
	cColorSpace.define_const("RGBX",       Rays:: RGBX);
	cColorSpace.define_const("ARGB",       Rays::ARGB);
	cColorSpace.define_const("XRGB",       Rays::XRGB);
	cColorSpace.define_const("BGRA",       Rays:: BGRA);
	cColorSpace.define_const("BGRX",       Rays:: BGRX);
	cColorSpace.define_const("ABGR",       Rays::ABGR);
	cColorSpace.define_const("XBGR",       Rays::XBGR);
}


namespace Rays
{


	Class
	color_space_class ()
	{
		return cColorSpace;
	}


}// Rays
