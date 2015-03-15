#include "rays/ruby/shader.h"


#include <rucy.h>
#include "defs.h"


using namespace Rucy;


RUCY_DEFINE_VALUE_FROM_TO(Rays::Shader)

#define THIS  to<Rays::Shader*>(self)

#define CHECK RUCY_CHECK_OBJECT(Rays::Shader, self)


static
RUCY_DEF_ALLOC(alloc, klass)
{
	return new_type<Rays::Shader>(klass);
}
RUCY_END

static
RUCY_DEF1(initialize, source)
{
	RUCY_CHECK_OBJ(Rays::Shader, self);

	if (!source.is_s())
		argument_error(__FILE__, __LINE__);

	*THIS = Rays::Shader(source.c_str());
	return self;
}
RUCY_END


static Class cShader;

void
Init_shader ()
{
	Module mRays = define_module("Rays");

	cShader = mRays.define_class("Shader");
	cShader.define_alloc_func(alloc);
	cShader.define_private_method("initialize", initialize);
}


namespace Rays
{


	Class
	shader_class ()
	{
		return cShader;
	}


}// Rays
