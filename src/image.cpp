#include "rays/image.h"


#include <assert.h>
#include "rays/exception.h"
#include "rays/bitmap.h"
#include "rays/texture.h"
#include "opengl.h"


namespace Rays
{


	struct Image::Data
	{

		int width, height;

		ColorSpace color_space;

		bool alpha_only;

		mutable Bitmap bitmap;

		mutable Texture texture;

		Data ()
		:	width(0), height(0), alpha_only(false)
		{
		}

	};// Image::Data


	Image::Image ()
	{
	}

	Image::Image (int width, int height, const ColorSpace& cs, bool alpha_only)
	{
		self->width       = width;
		self->height      = height;
		self->color_space = cs;
		self->alpha_only  = alpha_only;
	}

	Image::Image (const Bitmap& bitmap, bool alpha_only)
	{
		self->width       = bitmap.width();
		self->height      = bitmap.height();
		self->color_space = bitmap.color_space();
		self->alpha_only  = alpha_only;
		self->bitmap      = bitmap;
	}

	Image::~Image ()
	{
	}

	Image
	Image::copy () const
	{
		return Image(width(), height(), color_space(), alpha_only());
	}

	Painter
	Image::painter ()
	{
		Painter p;
		p.bind(texture());
		return p;
	}

	int
	Image::width () const
	{
		return self->width;
	}

	int
	Image::height () const
	{
		return self->height;
	}

	const ColorSpace&
	Image::color_space () const
	{
		return self->color_space;
	}

	bool
	Image::alpha_only () const
	{
		return self->alpha_only;
	}

	Bitmap&
	Image::bitmap ()
	{
		if (!self->bitmap)
		{
			if (self->texture)
				self->bitmap = Bitmap(self->texture);
			else
				self->bitmap = Bitmap(self->width, self->height, self->color_space);
		}
		else if (self->texture && !self->bitmap.dirty() && self->texture.dirty())
			self->bitmap = Bitmap(self->texture);

		if (self->bitmap)  self->bitmap.set_dirty(false);
		if (self->texture) self->texture.set_dirty(false);

		return self->bitmap;
	}

	const Bitmap&
	Image::bitmap () const
	{
		return const_cast<This*>(this)->bitmap();
	}

	static void
	invalidate_texture_for_another_context (Image* image)
	{
		assert(image);

		const Texture& tex = image->self->texture;
		if (!tex) return;

		Context get_context_for_texture (const Texture&);
		Context tex_context = get_context_for_texture(tex);
		if (!tex_context) return;

		Context current_context = get_context();
		if (tex_context == current_context)
			return;

		set_context(tex_context);
		image->bitmap();
		image->self->texture = Texture();

		set_context(current_context);
	}

	Texture&
	Image::texture ()
	{
		invalidate_texture_for_another_context(this);

		if (!self->texture)
		{
			if (self->bitmap)
				self->texture = Texture(self->bitmap, self->alpha_only);
			else
			{
				self->texture = Texture(
					self->width, self->height, self->color_space, self->alpha_only);

				Painter p;
				p.bind(self->texture);
				p.begin();
				p.clear();
				p.end();
			}
		}
		else if (self->bitmap && !self->texture.dirty() && self->bitmap.dirty())
			self->texture = Texture(self->bitmap, self->alpha_only);

		if (self->bitmap)  self->bitmap.set_dirty(false);
		if (self->texture) self->texture.set_dirty(false);

		return self->texture;
	}

	const Texture&
	Image::texture () const
	{
		return const_cast<This*>(this)->texture();
	}

	Image::operator bool () const
	{
		return self->width > 0 && self->height > 0 && self->color_space;
	}

	bool
	Image::operator ! () const
	{
		return !operator bool();
	}


	Image
	load_image (const char* path, bool alphatex)
	{
		return Image(load_bitmap(path), alphatex);
	}

	void
	save_image (const Image& image, const char* path)
	{
		if (!image)
			argument_error(__FILE__, __LINE__);

		return save_bitmap(image.bitmap(), path);
	}


}// Rays
