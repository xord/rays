// -*- c++ -*-
#pragma once
#ifndef __RAYS_SRC_POINT_H__
#define __RAYS_SRC_POINT_H__


#include <rays/point.h>
#include "coord.h"


namespace Rays
{


	inline       Point& to_rays(      Vec3& val) {return *(      Point*) &val;}

	inline const Point& to_rays(const Vec3& val) {return *(const Point*) &val;}


}// Rays


#endif//EOH
