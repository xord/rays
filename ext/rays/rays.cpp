#include "rays/ruby/rays.h"


#include <vector>
#include "defs.h"


RUCY_DEFINE_CONVERT_TO(Rays::CapType)
RUCY_DEFINE_CONVERT_TO(Rays::JoinType)


template <typename T>
struct EnumType
{
	const char* name;
	const char* short_name;
	T type;
};

static std::vector<EnumType<Rays::CapType>> CAP_TYPES({
	{"CAP_BUTT",   "BUTT",   Rays::CAP_BUTT},
	{"CAP_ROUND",  "ROUND",  Rays::CAP_ROUND},
	{"CAP_SQUARE", "SQUARE", Rays::CAP_SQUARE},
});

static std::vector<EnumType<Rays::JoinType>> JOIN_TYPES({
	{"JOIN_MITER",  "MITER",  Rays::JOIN_MITER},
	{"JOIN_ROUND",  "ROUND",  Rays::JOIN_ROUND},
	{"JOIN_SQUARE", "SQUARE", Rays::JOIN_SQUARE},
});


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

	for (auto it = CAP_TYPES.begin(); it != CAP_TYPES.end(); ++it)
		mRays.define_const(it->name, it->type);

	for (auto it = JOIN_TYPES.begin(); it != JOIN_TYPES.end(); ++it)
		mRays.define_const(it->name, it->type);
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
				for (auto it = CAP_TYPES.begin(); it != CAP_TYPES.end(); ++it)
				{
					if (
						strcasecmp(str, it->name)       == 0 ||
						strcasecmp(str, it->short_name) == 0)
					{
						return it->type;
					}
				}
			}
		}

		int type = value_to<int>(*argv, convert);
		if (type < 0 || Rays::CAP_MAX <= type)
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
				for (auto it = JOIN_TYPES.begin(); it != JOIN_TYPES.end(); ++it)
				{
					if (
						strcasecmp(str, it->name)       == 0 ||
						strcasecmp(str, it->short_name) == 0)
					{
						return it->type;
					}
				}
			}
		}

		int type = value_to<int>(*argv, convert);
		if (type < 0 || Rays::JOIN_MAX <= type)
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
