#include "rays/ruby/texture.h"


#include <rucy.h>
#include "rays/ruby/color_space.h"
#include "rays/ruby/bitmap.h"
#include "defs.h"


using namespace Rucy;

using Rays::coord;


RUCY_DEFINE_VALUE_FROM_TO(Rays::Texture)

#define THIS  to<Rays::Texture*>(self)

#define CHECK RUCY_CHECK_OBJECT(Rays::Texture, self)


static
RUCY_DEF_ALLOC(alloc, klass)
{
	return new_type<Rays::Texture>(klass);
}
RUCY_END

static
RUCY_DEFN(initialize)
{
	RUCY_CHECK_OBJ(Rays::Texture, self);
	check_arg_count(__FILE__, __LINE__, "Texture#initialize", argc, 1, 2, 3, 4);

	if (argv[0].is_kind_of(Rays::bitmap_class()))
	{
		check_arg_count(__FILE__, __LINE__, "Image#initialize", argc, 1, 2);

		const Rays::Bitmap* bitmap = to<Rays::Bitmap*>(argv[0]);
		if (!bitmap)
			argument_error(__FILE__, __LINE__);

		bool alpha_only = (argc >= 2) ? to<bool>(argv[1]) : false;
		*THIS = Rays::Texture(*bitmap, alpha_only);
	}
	else
	{
		check_arg_count(__FILE__, __LINE__, "Image#initialize", argc, 2, 3, 4);

		int width           = to<int>(argv[0]);
		int height          = to<int>(argv[1]);
		Rays::ColorSpace cs = (argc >= 3) ? to<Rays::ColorSpace>(argv[2]) : Rays::RGBA;
		bool alpha_only     = (argc >= 4) ? to<bool>(argv[3])             : false;
		*THIS = Rays::Texture(width, height, cs, alpha_only);
	}

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
RUCY_DEF0(alpha_only)
{
	CHECK;
	return value(THIS->alpha_only());
}
RUCY_END

static
RUCY_DEF1(s, x)
{
	CHECK;
	return value(THIS->s(x.as_f(true)));
}
RUCY_END

static
RUCY_DEF1(t, y)
{
	CHECK;
	return value(THIS->t(y.as_f(true)));
}
RUCY_END

static
RUCY_DEF0(s_max)
{
	CHECK;
	return value(THIS->s_max());
}
RUCY_END

static
RUCY_DEF0(t_max)
{
	CHECK;
	return value(THIS->t_max());
}
RUCY_END

static
RUCY_DEF0(to_bitmap)
{
	CHECK;
	return value(Rays::Bitmap(*THIS));
}
RUCY_END


static Class cTexture;

void
Init_texture ()
{
	Module mRays = define_module("Rays");

	cTexture = mRays.define_class("Texture");
	cTexture.define_alloc_func(alloc);
	cTexture.define_private_method("initialize", initialize);
	cTexture.define_method("width", width);
	cTexture.define_method("height", height);
	cTexture.define_method("color_space", color_space);
	cTexture.define_method("alpha_only", alpha_only);
	cTexture.define_method("s", s);
	cTexture.define_method("t", t);
	cTexture.define_method("s_max", s_max);
	cTexture.define_method("t_max", t_max);
	cTexture.define_method("to_bitmap", to_bitmap);
}


namespace Rays
{


	Class
	texture_class ()
	{
		return cTexture;
	}


}// Rays
