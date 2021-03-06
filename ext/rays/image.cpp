#include "rays/ruby/image.h"


#include <rucy.h>
#include "rays/ruby/color_space.h"
#include "rays/ruby/bitmap.h"
#include "rays/ruby/texture.h"
#include "rays/ruby/painter.h"
#include "defs.h"


using namespace Rucy;

using Rays::coord;


RUCY_DEFINE_VALUE_FROM_TO(Rays::Image)

#define THIS  to<Rays::Image*>(self)

#define CHECK RUCY_CHECK_OBJECT(Rays::Image, self)


static
RUCY_DEF_ALLOC(alloc, klass)
{
	return new_type<Rays::Image>(klass);
}
RUCY_END

static
RUCY_DEFN(initialize)
{
	RUCY_CHECK_OBJ(Rays::Image, self);
	check_arg_count(__FILE__, __LINE__, "Image#initialize", argc, 1, 2, 3);

	if (argc == 0) return self;

	if (argv[0].is_kind_of(Rays::bitmap_class()))
	{
		check_arg_count(__FILE__, __LINE__, "Image#initialize", argc, 1, 2);

		const Rays::Bitmap* bitmap = to<Rays::Bitmap*>(argv[0]);
		if (!bitmap) argument_error(__FILE__, __LINE__);

		bool alpha_only = (argc == 2) ? to<bool>(argv[1]) : false;
		*THIS = Rays::Image(*bitmap, alpha_only);
	}
	else
	{
		int width  = to<int>(argv[0]);
		int height = to<int>(argv[1]);
		uint colorspace = (argc == 3) ? to<uint>(argv[2]) : (uint) Rays::RGBA;
		*THIS = Rays::Image(width, height, (Rays::ColorSpaceType) colorspace);
	}

	return self;
}
RUCY_END

static
RUCY_DEF1(initialize_copy, obj)
{
	RUCY_CHECK_OBJ(Rays::Image, self);

	*THIS = to<Rays::Image&>(obj).copy();
	return self;
}
RUCY_END

static
RUCY_DEF0(painter)
{
	CHECK;
	return value(THIS->painter());
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
RUCY_DEF0(bitmap)
{
	CHECK;
	return value(THIS->bitmap());
}
RUCY_END

static
RUCY_DEF0(texture)
{
	CHECK;
	return value(THIS->texture());
}
RUCY_END

static
RUCY_DEF1(save, path)
{
	CHECK;
	Rays::save_image(*THIS, path.c_str());
	return self;
}
RUCY_END


static
RUCY_DEF2(load, path, alpha_only)
{
	return value(Rays::load_image(path.c_str(), to<bool>(alpha_only)));
}
RUCY_END


static Class cImage;

void
Init_image ()
{
	Module mRays = define_module("Rays");

	cImage = mRays.define_class("Image");
	cImage.define_alloc_func(alloc);
	cImage.define_private_method("initialize", initialize);
	cImage.define_private_method("initialize_copy", initialize_copy);
	cImage.define_method("painter", painter);
	cImage.define_method("width", width);
	cImage.define_method("height", height);
	cImage.define_method("color_space", color_space);
	cImage.define_method("alpha_only", alpha_only);
	cImage.define_method("bitmap", bitmap);
	cImage.define_method("texture", texture);
	cImage.define_method("save", save);
	cImage.define_function("load_image", load);
}


namespace Rays
{


	Class
	image_class ()
	{
		return cImage;
	}


}// Rays
