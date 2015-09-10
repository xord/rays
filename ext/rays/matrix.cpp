#include "rays/ruby/matrix.h"


#include <rucy.h>
#include "defs.h"


using namespace Rucy;


RUCY_DEFINE_VALUE_FROM_TO(Rays::Matrix)

#define THIS  to<Rays::Matrix*>(self)

#define CHECK RUCY_CHECK_OBJ(Rays::Matrix, self)


static
RUCY_DEF_ALLOC(alloc, klass)
{
	return new_type<Rays::Matrix>(klass);
}
RUCY_END

static
RUCY_DEFN(initialize)
{
	CHECK;
	check_arg_count(__FILE__, __LINE__, "Matrix#initialize", argc, 0, 1, 16);

	if (argc > 0)
		*THIS = to<Rays::Matrix>(argc, argv);

	return self;
}
RUCY_END

static
RUCY_DEF1(initialize_copy, obj)
{
	CHECK;
	*THIS = to<Rays::Matrix&>(obj);
	return self;
}
RUCY_END

static
RUCY_DEFN(set)
{
	CHECK;
	check_arg_count(__FILE__, __LINE__, "Matrix#initialize", argc, 0, 1, 16);

	switch (argc)
	{
		case 0:
			*THIS = Rays::Matrix();
			break;

		case 1:
			*THIS = Rays::Matrix(to<float>(argv[0]));
			break;

		case 16:
			*THIS = Rays::Matrix(
				to<float>(argv[0]),  to<float>(argv[1]),  to<float>(argv[2]),  to<float>(argv[3]),
				to<float>(argv[4]),  to<float>(argv[5]),  to<float>(argv[6]),  to<float>(argv[7]),
				to<float>(argv[8]),  to<float>(argv[9]),  to<float>(argv[10]), to<float>(argv[11]),
				to<float>(argv[12]), to<float>(argv[13]), to<float>(argv[14]), to<float>(argv[15]));
			break;
	}

	return self;
}
RUCY_END

static
RUCY_DEF2(at, row, column)
{
	CHECK;
	return value(THIS->at(row.as_i(), column.as_i()));
}
RUCY_END


static Class cMatrix;

void
Init_matrix ()
{
	Module mRays = define_module("Rays");

	cMatrix = mRays.define_class("Matrix");
	cMatrix.define_alloc_func(alloc);
	cMatrix.define_private_method("initialize",      initialize);
	cMatrix.define_private_method("initialize_copy", initialize_copy);
	cMatrix.define_method("set", set);
	cMatrix.define_method("at", at);
}


namespace Rucy
{


	template <> Rays::Matrix
	value_to<Rays::Matrix> (int argc, const Value* argv, bool convert)
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
				return Rays::Matrix();
			else if (argv->is_i() || argv->is_f())
			{
				switch (argc)
				{
					#define V(i) argv[i].as_f(true)
					case 1:  return Rays::Matrix(V(0));
					case 16: return Rays::Matrix(
						V(0),  V(1),  V(2),  V(3),
						V(4),  V(5),  V(6),  V(7),
						V(8),  V(9),  V(10), V(11),
						V(12), V(13), V(14), V(15));
					#undef V
					default: argument_error(__FILE__, __LINE__);
				}
			}
		}

		if (argc != 1)
			argument_error(__FILE__, __LINE__);

		return value_to<Rays::Matrix&>(*argv, convert);
	}


}// Rucy


namespace Rays
{


	Class
	matrix_class ()
	{
		return cMatrix;
	}


}// Rays
