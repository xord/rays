// -*- c++ -*-
#pragma once
#ifndef __RAYS_RUBY_TEXTURE_H__
#define __RAYS_RUBY_TEXTURE_H__


#include <rucy/rucy.h>
#include <rucy/class.h>
#include <rucy/extension.h>
#include <rays/texture.h>


namespace Rays
{


	Rucy::Class texture_class ();
	// class Rays::Texture


}// Rays


RUCY_DECLARE_VALUE_FROM_TO(Rays::Texture)


namespace Rucy
{


	template <> inline Class
	get_ruby_class<Rays::Texture> ()
	{
		return Rays::texture_class();
	}


}// Rucy


#endif//EOH
