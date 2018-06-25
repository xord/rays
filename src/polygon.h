// -*- c++ -*-
#pragma once
#ifndef __RAYS_SRC_POLYGON_H__
#define __RAYS_SRC_POLYGON_H__


#include <vector>
#include <rays/polygon.h>


namespace Rays
{


	enum
	{

		NSEGMENT_ROUND   = 8,

		NSEGMENT_ELLIPSE = NSEGMENT_ROUND * 4

	};


	void Polygon_triangulate (
		std::vector<Point>* triangles, const Polygon& polygon);


}// Rays


#endif//EOH
