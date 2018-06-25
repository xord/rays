// -*- c++ -*-
#pragma once
#ifndef __RAYS_SRC_POLYGON_H__
#define __RAYS_SRC_POLYGON_H__


#include <vector>
#include <deque>
#include <boost/polygon/polygon.hpp>
#include <rays/polygon.h>


namespace Rays
{


	typedef double                                              BoostCoord;

	typedef boost::polygon::point_data<BoostCoord>              BoostPoint;

	typedef boost::polygon::polygon_with_holes_data<BoostCoord> BoostPolygon;

	typedef boost::polygon::polygon_set_data<BoostCoord>        BoostPolygonSet;

	typedef std::vector<BoostPolygon>                           BoostPolygonList;


	const BoostPolygonList& Polygon_subdivide (const Polygon& polygon);

	const BoostPolygonSet& Polygon_get_boost_polygons (const Polygon& polygon);


}// Rays


#endif//EOH
