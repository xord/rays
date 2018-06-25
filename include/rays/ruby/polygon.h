// -*- c++ -*-
#pragma once
#ifndef __RAYS_RUBY_POLYGON_H__
#define __RAYS_RUBY_POLYGON_H__


#include <rucy/rucy.h>
#include <rucy/class.h>
#include <rucy/extension.h>
#include <rays/polygon.h>


namespace Rays
{


	Rucy::Class polygon_class ();
	// class Rays::Polygon


}// Rays


RUCY_DECLARE_VALUE_OR_ARRAY_FROM_TO(Rays::Polygon)


namespace Rucy
{


	template <> inline Class
	get_ruby_class<Rays::Polygon> ()
	{
		return Rays::polygon_class();
	}


}// Rucy


#endif//EOH
