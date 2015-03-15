// -*- c++ -*-
#pragma once
#ifndef __RAYS_SHADER_H__
#define __RAYS_SHADER_H__


#include <xot/pimpl.h>
#include "rays/opengl.h"


namespace Rays
{


	class Shader
	{

		typedef Shader This;

		public:

			Shader ();

			Shader (const char* source);

			~Shader ();

			GLuint id () const;

			bool operator == (const Shader& rhs) const;

			bool operator != (const Shader& rhs) const;

			operator bool () const;

			bool operator ! () const;

			struct Data;

			Xot::PImpl<Data, true> self;

	};// Shader


}// Rays


#endif//EOH
