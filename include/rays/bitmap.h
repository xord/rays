// -*- c++ -*-
#pragma once
#ifndef __RAYS_BITMAP_H__
#define __RAYS_BITMAP_H__


#include <xot/pimpl.h>
#include <rays/defs.h>
#include <rays/color_space.h>
#include <rays/font.h>


namespace Rays
{


	class Texture;


	class Bitmap
	{

		typedef Bitmap This;

		public:

			Bitmap ();

			Bitmap (
				int width, int height, const ColorSpace& cs = RGBA,
				const void* pixels = NULL);

			Bitmap (const Texture& texture);

			~Bitmap ();

			Bitmap copy () const;

			int width () const;

			int height () const;

			const ColorSpace& color_space () const;

			int pitch () const;

			size_t size () const;

			      void* pixels ();

			const void* pixels () const;

			template <typename T>       T* at (int x, int y)
			{
				return (T*) (((char*) pixels()) + pitch() * y + x * color_space().Bpp());
			}

			template <typename T> const T* at (int x, int y) const
			{
				return const_cast<This*>(this)->at<T>(x, y);
			}

			bool     dirty () const;

			void set_dirty (bool b = true);

			operator bool () const;

			bool operator ! () const;

			struct Data;

			Xot::PSharedImpl<Data> self;

	};// Bitmap


	Bitmap load_bitmap (const char* path);

	void save_bitmap (const Bitmap& bitmap, const char* path);


	void draw_string (
		Bitmap* bitmap, const char* str,
		coord x = 0, coord y = 0, const Font& font = default_font());


}// Rays


#endif//EOH
