#include "rays/ruby/rays.h"


#include "defs.h"


RUCY_DEFINE_VALUE_OR_ARRAY_TO(Rays::CapType)
RUCY_DEFINE_VALUE_OR_ARRAY_TO(Rays::JoinType)


static struct CapTypeEnum
{
	const char* name;
	Rays::CapType type;
}
CAP_TYPES[] =
{
	{"CAP_BUTT",   Rays::CAP_BUTT},
	{"CAP_ROUND",  Rays::CAP_ROUND},
	{"CAP_SQUARE", Rays::CAP_SQUARE},
};

static const size_t CAP_TYPES_SIZE =
	sizeof(CAP_TYPES) / sizeof(CAP_TYPES[0]);


static struct JoinTypeEnum
{
	const char* name;
	Rays::JoinType type;
}
JOIN_TYPES[] =
{
	{"JOIN_MITER",  Rays::JOIN_MITER},
	{"JOIN_ROUND",  Rays::JOIN_ROUND},
	{"JOIN_SQUARE", Rays::JOIN_SQUARE},
};

static const size_t JOIN_TYPES_SIZE =
	sizeof(JOIN_TYPES) / sizeof(JOIN_TYPES[0]);


static
RUCY_DEF0(init)
{
	Rays::init();
	return self;
}
RUCY_END

static
RUCY_DEF0(fin)
{
	Rays::fin();
	return self;
}
RUCY_END


static Module mRays;

void
Init_rays ()
{
	mRays = define_module("Rays");

	mRays.define_singleton_method("init!", init);
	mRays.define_singleton_method("fin!", fin);

	for (size_t i = 0; i < CAP_TYPES_SIZE; ++i)
		mRays.define_const(CAP_TYPES[i].name, CAP_TYPES[i].type);

	for (size_t i = 0; i < JOIN_TYPES_SIZE; ++i)
		mRays.define_const(JOIN_TYPES[i].name, JOIN_TYPES[i].type);
}


namespace Rucy
{


	template <> Rays::CapType
	value_to<Rays::CapType> (int argc, const Value* argv, bool convert)
	{
		assert(argc > 0 && argv);

		if (convert)
		{
			if (argv->is_s() || argv->is_sym())
			{
				const char* str = argv->c_str();
				for (size_t i = 0; i < CAP_TYPES_SIZE; ++i)
				{
					if (strcasecmp(str, CAP_TYPES[i].name) == 0)
						return CAP_TYPES[i].type;
				}
			}
		}

		uint type = value_to<uint>(*argv, convert);
		if (type >= Rays::CAP_MAX)
			argument_error(__FILE__, __LINE__, "invalid cap type -- %d", type);

		return (Rays::CapType) type;
	}


	template <> Rays::JoinType
	value_to<Rays::JoinType> (int argc, const Value* argv, bool convert)
	{
		assert(argc > 0 && argv);

		if (convert)
		{
			if (argv->is_s() || argv->is_sym())
			{
				const char* str = argv->c_str();
				for (size_t i = 0; i < JOIN_TYPES_SIZE; ++i)
				{
					if (strcasecmp(str, JOIN_TYPES[i].name) == 0)
						return JOIN_TYPES[i].type;
				}
			}
		}

		uint type = value_to<uint>(*argv, convert);
		if (type >= Rays::JOIN_MAX)
			argument_error(__FILE__, __LINE__, "invalid join type -- %d", type);

		return (Rays::JoinType) type;
	}


}// Rucy


namespace Rays
{


	Module
	rays_module ()
	{
		return mRays;
	}


}// Rays
