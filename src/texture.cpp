#include "rays/texture.h"


#include "rays/exception.h"
#include "rays/bitmap.h"
#include "rays/opengl.h"
#include "frame_buffer.h"


namespace Rays
{


	struct Texture::Data
	{

		int id, width, height, width_pow2, height_pow2;

		ColorSpace color_space;

		bool alpha_only, dirty;

		Data ()
		:	id(-1)
		{
			clear();
		}

		~Data ()
		{
			clear();
		}

		void clear ()
		{
			if (id >= 0)
			{
				GLenum id_ = id;
				glDeleteTextures(1, &id_);
			}

			id = -1;
			width = height = width_pow2 = height_pow2 = 0;
			color_space = COLORSPACE_UNKNOWN;
			alpha_only = dirty = false;
		}

	};// Texture::Data


	static void
	create_texture (
		Texture::Data* self, const Bitmap* bitmap, GLenum format, GLenum type)
	{
		if (!self || (bitmap && !*bitmap))
			argument_error(__FILE__, __LINE__);

		GLuint id = 0;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		if (glIsTexture(id) == GL_FALSE)
			opengl_error(__FILE__, __LINE__, "failed to create texture.");

		self->id = id;

		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);//GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);//GL_LINEAR);

		glTexImage2D(
			GL_TEXTURE_2D, 0, format, self->width_pow2, self->height_pow2, 0,
			format, type, bitmap ? bitmap->pixels() : NULL);

		check_error(__FILE__, __LINE__);
	}

	static int
	min_pow2 (int num)
	{
		int n = 1;
		while (n < num) n *= 2;
		return n;
	}

	static void
	colorspace_for_alphabitmap (ColorSpace* result, ColorSpace cs)
	{
		if (!result || !cs)
			argument_error(__FILE__, __LINE__);

		*result = COLORSPACE_UNKNOWN;

		if (cs.is_float())
			*result = GRAY_float;
		else
		{
			switch (cs.bpc())
			{
				case 8:  *result = GRAY_8;  break;
				case 16: *result = GRAY_16; break;
				case 24: *result = GRAY_24; break;
				case 32: *result = GRAY_32; break;
				default: rays_error(__FILE__, __LINE__, "invalid bpc.");
			}
		}
	}

	template <int BytesPerPixel>
	static inline void
	copy_pixel (uchar* dest, const uchar* src)
	{
		*dest = *src;
		copy_pixel<BytesPerPixel - 1>(dest + 1, src + 1);
	}

	template <>
	inline void
	copy_pixel<1> (uchar* dest, const uchar* src)
	{
		*dest = *src;
	}

	template <>
	inline void
	copy_pixel<2> (uchar* dest, const uchar* src)
	{
		assert(sizeof(ushort) == 2);
		*(ushort*) dest = *(ushort*) src;
	}

	template <>
	inline void
	copy_pixel<4> (uchar* dest, const uchar* src)
	{
		assert(sizeof(uint) == 4);
		*(uint*) dest = *(uint*) src;
	}

	template <int BytesPerPixel>
	static inline void
	copy_pixels (
		size_t width,
		uchar* dest, size_t dest_offset, const uchar* src, size_t src_offset)
	{
		if (!dest || !src || dest_offset <= 0 || src_offset <= 0)
			argument_error(__FILE__, __LINE__);

		while (width--)
		{
			copy_pixel<BytesPerPixel>(dest, src);
			dest += dest_offset;
			src  += src_offset;
		}
	}

	static inline void
	copy_pixels (
		size_t Bpp, size_t width,
		uchar* dest, size_t dest_offset, const uchar* src, size_t src_offset)
	{
		switch (Bpp)
		{
			case 1: copy_pixels<1>(width, dest, dest_offset, src, src_offset); break;
			case 2: copy_pixels<2>(width, dest, dest_offset, src, src_offset); break;
			case 3: copy_pixels<3>(width, dest, dest_offset, src, src_offset); break;
			case 4: copy_pixels<4>(width, dest, dest_offset, src, src_offset); break;
		}
	}

	static void
	copy_bitmap (Bitmap* dest, const Bitmap& src, int src_offset = 0)
	{
		if (!dest || !src || src_offset < 0)
			argument_error(__FILE__, __LINE__);

		int src_Bpp  = src.color_space().Bpp();
		int src_Bpc  = src.color_space().Bpc();
		int dest_Bpp = dest->color_space().Bpp();

		if (src_offset >= (src_Bpp / src_Bpc))
			rays_error(__FILE__, __LINE__, "invalid src_offset.");

		int width  = std::min(src.width(),  dest->width());
		int height = std::min(src.height(), dest->height());

		for (int y = 0; y < height; ++y)
		{
			const uchar* s = src.  at<uchar>(0, y) + src_offset * src_Bpc;
			      uchar* d = dest->at<uchar>(0, y);
			copy_pixels(src_Bpp, width, d, dest_Bpp, s, src_Bpp);
		}
	}

	static void
	setup_texture (
		Texture::Data* self, int width, int height, const ColorSpace& cs,
		const Bitmap* bitmap, bool alpha_only)
	{
		if (!self || width <= 0 || height <= 0 || !cs || (bitmap && !*bitmap))
			argument_error(__FILE__, __LINE__);

		GLenum format, type;
		cs.get_gl_enums(&format, &type, alpha_only);

		self->clear();

		self->width       = width;
		self->height      = height;
		self->width_pow2  = min_pow2(width);
		self->height_pow2 = min_pow2(height);
		self->color_space = cs;
		self->alpha_only  = alpha_only;
		self->dirty       = true;

		if (alpha_only)
		{
			colorspace_for_alphabitmap(&self->color_space, cs);
			if (!self->color_space.is_gray())
				rays_error(__FILE__, __LINE__, "alpha_only takes only gray color-space.");
		}

		Bitmap bmp;
		if (bitmap)
		{
			if (
				self->color_space != cs ||
				self->width_pow2  != self->width ||
				self->height_pow2 != self->height)
			{
				bmp = Bitmap(self->width_pow2, self->height_pow2, self->color_space);
				if (!bmp)
					rays_error(__FILE__, __LINE__, "creating bitmap failed.");

				copy_bitmap(&bmp, *bitmap, alpha_only ? self->color_space.alpha_pos() : 0);
				bitmap = &bmp;
			}
		}

		create_texture(self, bitmap, format, type);
	}


	Texture::Texture ()
	{
	}

	Texture::Texture (int width, int height, const ColorSpace& cs, bool alpha_only)
	{
		setup_texture(
			self.get(), width, height, cs,
			NULL, alpha_only);
	}

	Texture::Texture (const Bitmap& bitmap, bool alpha_only)
	{
		setup_texture(
			self.get(), bitmap.width(), bitmap.height(), bitmap.color_space(),
			&bitmap, alpha_only);
	}

	Texture::~Texture ()
	{
	}

	GLuint
	Texture::id () const
	{
		return self->id;
	}

	int
	Texture::width () const
	{
		return self->width;
	}

	int
	Texture::height () const
	{
		return self->height;
	}

	const ColorSpace&
	Texture::color_space () const
	{
		return self->color_space;
	}

	bool
	Texture::alpha_only () const
	{
		return self->alpha_only;
	}

	float
	Texture::s (float x) const
	{
		return x / (float) self->width_pow2;
	}

	float
	Texture::t (float y) const
	{
		return y / (float) self->height_pow2;
	}

	float
	Texture::s_max () const
	{
		return s(self->width);
	}

	float
	Texture::t_max () const
	{
		return t(self->height);
	}

	bool
	Texture::dirty () const
	{
		return self->dirty;
	}

	void
	Texture::set_dirty (bool b)
	{
		self->dirty = b;
	}

	Texture::operator bool () const
	{
		return self->id >= 0 && self->width > 0 && self->height > 0;
	}

	bool
	Texture::operator ! () const
	{
		return !operator bool();
	}


}// Rays
