#include "rays/ruby/polyline.h"


#include <vector>
#include <rucy.h>
#include "rays/ruby/point.h"
#include "defs.h"


using namespace Rucy;

using Rays::coord;


RUCY_DEFINE_VALUE_OR_ARRAY_FROM_TO(Rays::Polyline)

#define THIS  to<Rays::Polyline*>(self)

#define CHECK RUCY_CHECK_OBJECT(Rays::Polyline, self)


static
RUCY_DEF_ALLOC(alloc, klass)
{
	return new_type<Rays::Polyline>(klass);
}
RUCY_END

static
RUCY_DEF1(initialize_copy, obj)
{
	CHECK;
	*THIS = to<Rays::Polyline&>(obj).dup();
	return self;
}
RUCY_END

static
RUCY_DEF0(loop)
{
	CHECK;
	return value(THIS->loop());
}
RUCY_END

static
RUCY_DEF0(size)
{
	CHECK;
	return value(THIS->size());
}
RUCY_END

static
RUCY_DEF0(empty)
{
	CHECK;
	return value(THIS->empty());
}
RUCY_END

static
RUCY_DEF1(at, index)
{
	CHECK;
	return value((*THIS)[index]);
}
RUCY_END

static
RUCY_DEF0(each)
{
	CHECK;

	Value ret;
	for (const auto& point : *THIS)
		ret = rb_yield(value(point));
	return ret;
}
RUCY_END


static Class cPolyline;

void
Init_polyline ()
{
	Module mRays = define_module("Rays");

	cPolyline = mRays.define_class("Polyline");
	cPolyline.define_alloc_func(alloc);
	cPolyline.define_private_method("initialize_copy", initialize_copy);
	cPolyline.define_method("loop?",  loop);
	cPolyline.define_method("size",   size);
	cPolyline.define_method("empty?", empty);
	cPolyline.define_method("[]",     at);
	cPolyline.define_method("each",   each);
}


namespace Rays
{


	Class
	polyline_class ()
	{
		return cPolyline;
	}


}// Rays
