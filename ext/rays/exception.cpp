#include "rays/ruby/exception.h"


#include "defs.h"


static Class cRaysError;
static Class cOpenGLError;

void
Init_exception ()
{
	Module mRays = define_module("Rays");

	cRaysError   = mRays.define_class("RaysError", rb_eRuntimeError);
	cOpenGLError = mRays.define_class("OpenGLError", cRaysError);
}


namespace Rays
{


	Class
	rays_error_class ()
	{
		return cRaysError;
	}

	Class
	opengl_error_class ()
	{
		return cOpenGLError;
	}


}// Rays
