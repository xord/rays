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
RUCY_DEF1(setup, source)
{
	RUCY_CHECK_OBJ(Rays::Shader, self);

	*THIS = to<Rays::Shader>(source);
}
RUCY_END

static
RUCY_DEFN(set_uniform)
{
	CHECK;
	check_arg_count(__FILE__, __LINE__, "Painter#set_uniform", argc, 2, 3, 4, 5);

	#define Ai(n) (argv[n].as_i())
	#define Af(n) ((float) argv[n].as_f())

	const char* name = argv[0].c_str();
	if (argv[1].is_array())
	{
		argc = argv[1].size();
		argv = argv[1].as_array();
	}
	else
	{
		argc -= 1;
		argv += 1;
	}

	if (argv[0].is_i())
	{
		switch (argc)
		{
			case 1: THIS->set_uniform(name, Ai(0)); break;
			case 2: THIS->set_uniform(name, Ai(0), Ai(1)); break;
			case 3: THIS->set_uniform(name, Ai(0), Ai(1), Ai(2)); break;
			case 4: THIS->set_uniform(name, Ai(0), Ai(1), Ai(2), Ai(3)); break;
		}
	}
	else if (argv[0].is_f())
	{
		switch (argc)
		{
			case 1: THIS->set_uniform(name, Af(0)); break;
			case 2: THIS->set_uniform(name, Af(0), Af(1)); break;
			case 3: THIS->set_uniform(name, Af(0), Af(1), Af(2)); break;
			case 4: THIS->set_uniform(name, Af(0), Af(1), Af(2), Af(3)); break;
		}
	}
	else
		argument_error(__FILE__, __LINE__);

	#undef Ai
	#undef Af

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
	cShader.define_private_method("setup", setup);
	cShader.define_private_method("set_uniform", set_uniform);
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
