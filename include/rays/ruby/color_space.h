// -*- c++ -*-
#pragma once
#ifndef __RAYS_RUBY_COLOR_SPACE_H__
#define __RAYS_RUBY_COLOR_SPACE_H__


#include <rucy/class.h>
#include <rucy/extension.h>
#include <rays/color_space.h>


RUCY_DECLARE_VALUE_OR_ARRAY_FROM_TO(RAYS_EXPORT, Rays::ColorSpace)


namespace Rays
{


	RAYS_EXPORT Rucy::Class color_space_class ();
	// class Rays::ColorSpace


}// Rays


namespace Rucy
{


	template <> inline Class
	get_ruby_class<Rays::ColorSpace> ()
	{
		return Rays::color_space_class();
	}


}// Rucy


#endif//EOH
