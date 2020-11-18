#include "rays/ruby/camera.h"


#include "rays/ruby/image.h"
#include "defs.h"


RUCY_DEFINE_VALUE_FROM_TO(Rays::Camera)

#define THIS  to<Rays::Camera*>(self)

#define CHECK RUCY_CHECK_OBJECT(Rays::Camera, self)


static
RUCY_DEF_ALLOC(alloc, klass)
{
	return new_type<Rays::Camera>(klass);
}
RUCY_END

static
RUCY_DEFN(initialize)
{
	RUCY_CHECK_OBJ(Rays::Camera, self);

	*THIS = Rays::Camera();
	return self;
}
RUCY_END

static
RUCY_DEF0(start)
{
	CHECK;
	return value(THIS->start());
}
RUCY_END

static
RUCY_DEF0(stop)
{
	CHECK;
	THIS->stop();
}
RUCY_END

static
RUCY_DEF0(is_active)
{
	CHECK;
	return value(THIS->is_active());
}
RUCY_END

static
RUCY_DEF0(image)
{
	CHECK;
	const Rays::Image* img = THIS->image();
	return img ? value(*img) : nil();
}
RUCY_END


static Class cCamera;

void
Init_camera ()
{
	Module mRays = define_module("Rays");

	cCamera = mRays.define_class("Camera");
	cCamera.define_alloc_func(alloc);
	cCamera.define_private_method("initialize", initialize);
	cCamera.define_method("start",   start);
	cCamera.define_method("stop",    stop);
	cCamera.define_method("active?", is_active);
	cCamera.define_method("image",   image);
}


namespace Rays
{


	Class
	camera_class ()
	{
		return cCamera;
	}


}// Rays
