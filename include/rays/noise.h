// -*- c++ -*-
#pragma once
#ifndef __RAYS_NOISE_H__
#define __RAYS_NOISE_H__


#include <rays/defs.h>


namespace Rays
{


	struct Point;


	coord perlin (coord x);

	coord perlin (coord x, coord y);

	coord perlin (coord x, coord y, coord z);

	coord perlin (coord x, coord y, coord z, coord w);

	coord perlin (const Point& point);


	coord simplex (coord x);

	coord simplex (coord x, coord y);

	coord simplex (coord x, coord y, coord z);

	coord simplex (coord x, coord y, coord z, coord w);

	coord simplex (const Point& point);


}// Rays


#endif//EOH
