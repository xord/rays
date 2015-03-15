#include <rucy.h>
#include "rays/rays.h"
#include "rays/opengl.h"
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

static
RUCY_DEF0(init_offscreen_context)
{
	Rays::init_offscreen_context();
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
	mRays.define_singleton_method("init_offscreen_context", init_offscreen_context);
}


namespace Rays
{


	Module
	rays_module ()
	{
		return mRays;
	}


}// Rays
