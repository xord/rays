// -*- c++ -*-
#pragma once
#ifndef __RAYS_SRC_POINT_H__
#define __RAYS_SRC_POINT_H__


#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <rays/point.h>


namespace Rays
{	


	typedef glm::tvec2<coord> Vec2;

	typedef glm::tvec3<coord> Vec3;


	inline       Vec3&  to_glm(      Point& val) {return *(      Vec3*)  &val;}

	inline const Vec3&  to_glm(const Point& val) {return *(const Vec3*)  &val;}

	inline       Point& to_rays(      Vec3& val) {return *(      Point*) &val;}

	inline const Point& to_rays(const Vec3& val) {return *(const Point*) &val;}


}// Rays


#endif//EOH
