#include "rays/ruby/shader.h"


#include <rucy.h>
#include "defs.h"


using namespace Rucy;


RUCY_DEFINE_VALUE_OR_ARRAY_FROM_TO(Rays::Shader)

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

	*THIS = to<Rays::Shader>(source);
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


namespace Rucy
{


	template <> Rays::Shader
	value_to<Rays::Shader> (int argc, const Value* argv, bool convert)
	{
		if (argc == 1 && argv->is_array())
		{
			argc = argv->size();
			argv = argv->as_array();
		}

		assert(argc > 0 && argv);

		if (convert)
		{
			if (argv->is_s())
				return Rays::Shader(argv[0].c_str());
		}

		if (argc != 1)
			argument_error(__FILE__, __LINE__);

		return value_to<Rays::Shader&>(*argv, convert);
	}


}// Rucy


namespace Rays
{


	Class
	shader_class ()
	{
		return cShader;
	}


}// Rays
