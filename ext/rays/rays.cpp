#include <rucy.h>
#include "rays/rays.h"
#include "defs.h"


using namespace Rucy;


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
}


namespace Rays
{


	Module
	rays_module ()
	{
		return mRays;
	}


}// Rays
