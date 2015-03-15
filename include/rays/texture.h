// -*- c++ -*-
#pragma once
#ifndef __RAYS_TEXTURE_H__
#define __RAYS_TEXTURE_H__


#include <xot/pimpl.h>
#include <rays/defs.h>
#include <rays/color_space.h>
#include <rays/opengl.h>


namespace Rays
{


	class Bitmap;


	class Texture
	{

		public:

			Texture ();

			Texture (
				int width, int height, const ColorSpace& cs = RGBA,
				bool alpha_only = false);

			Texture (const Bitmap& bitmap, bool alpha_only = false);

			~Texture ();

			GLuint id () const;

			int width () const;

			int height () const;

			const ColorSpace& color_space () const;

			bool alpha_only () const;

			float s (float x) const;

			float t (float y) const;

			float s_max () const;

			float t_max () const;

			bool     dirty () const;

			void set_dirty (bool b = true);

			operator bool () const;

			bool operator ! () const;

			struct Data;

			Xot::PImpl<Data, true> self;

	};// Texture


}// Rays


#endif//EOH
