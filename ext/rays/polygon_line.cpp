#include "rays/ruby/polygon.h"


#include "rays/ruby/polyline.h"
#include "defs.h"


RUCY_DEFINE_VALUE_OR_ARRAY_FROM_TO(Rays::Polygon::Line)

#define THIS  to<Rays::Polygon::Line*>(self)

#define CHECK RUCY_CHECK_OBJECT(Rays::Polygon::Line, self)


static
RUCY_DEF_ALLOC(alloc, klass)
{
	not_implemented_error(__FILE__, __LINE__);
}
RUCY_END

static
RUCY_DEF0(hole)
{
	CHECK;
	return value(THIS->hole());
}
RUCY_END


static Class cPolygonLine;

void
Init_polygon_line ()
{
	Module mRays   = define_module("Rays");
	Class cPolygon = mRays.define_class("Polygon");

	cPolygonLine = cPolygon.define_class("Line", Rays::polyline_class());
	cPolygonLine.define_alloc_func(alloc);
	cPolygonLine.define_method("hole?", hole);
}


namespace Rays
{


	Class
	polygon_line_class ()
	{
		return cPolygonLine;
	}


}// Rays
