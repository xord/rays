// -*- c++ -*-
#pragma once
#ifndef __RAYS_IMAGE_H__
#define __RAYS_IMAGE_H__


#include <xot/pimpl.h>
#include <rays/color_space.h>
#include <rays/painter.h>


namespace Rays
{


	class Bitmap;

	class Texture;


	class Image
	{

		typedef Image This;

		public:

			Image ();

			Image (
				int width, int height, const ColorSpace& cs = RGBA,
				bool alpha_only = false);

			Image (const Bitmap& bitmap, bool alpha_only = false);

			~Image ();

			Image copy () const;

			Painter painter ();

			int width () const;

			int height () const;

			const ColorSpace& color_space () const;

			bool alpha_only () const;

			      Bitmap& bitmap ();

			const Bitmap& bitmap () const;

			      Texture& texture ();

			const Texture& texture () const;

			operator bool () const;

			bool operator ! () const;

			struct Data;

			Xot::PSharedImpl<Data> self;

	};// Image


	Image load_image (const char* path, bool alphatexture = false);

	void save_image (const Image& image, const char* path);


}// Rays


#endif//EOH
