// -*- c++ -*-
#pragma once
#ifndef __RAYS_SHADER_H__
#define __RAYS_SHADER_H__


#include <xot/pimpl.h>
#include <rays/defs.h>
#include <rays/coord.h>


namespace Rays
{


	class Image;


	class Shader
	{

		typedef Shader This;

		public:

			Shader (const char* source = NULL);

			virtual ~Shader ();

			virtual void set_uniform (const char* name, int arg1);

			virtual void set_uniform (const char* name, int arg1, int arg2);

			virtual void set_uniform (const char* name, int arg1, int arg2, int arg3);

			virtual void set_uniform (const char* name, int arg1, int arg2, int arg3, int arg4);

			virtual void set_uniform (const char* name, const int* args, size_t size);

			virtual void set_uniform (const char* name, float arg1);

			virtual void set_uniform (const char* name, float arg1, float arg2);

			virtual void set_uniform (const char* name, float arg1, float arg2, float arg3);

			virtual void set_uniform (const char* name, float arg1, float arg2, float arg3, float arg4);

			virtual void set_uniform (const char* name, const float* args, size_t size);

			virtual void set_uniform (const char* name, const Coord2& vec2);

			virtual void set_uniform (const char* name, const Coord3& vec3);

			virtual void set_uniform (const char* name, const Coord4& vec4);

			virtual void set_uniform (const char* name, const Image& texture);

			virtual operator bool () const;

			virtual bool operator ! () const;

			friend bool operator == (const This& lhs, const This& rhs);

			friend bool operator != (const This& lhs, const This& rhs);

			struct Data;

			Xot::PSharedImpl<Data> self;

	};// Shader


}// Rays


#endif//EOH
