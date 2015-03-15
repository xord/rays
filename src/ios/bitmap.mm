// -*- objc -*-
#import "rays/bitmap.h"


#include <assert.h>
#include <boost/scoped_array.hpp>
#import <ImageIO/ImageIO.h>
#import <MobileCoreServices/UTCoreTypes.h>
#include "rays/exception.h"
#include "rays/texture.h"
#include "../frame_buffer.h"
#include "helper.h"


namespace Rays
{


	static CGBitmapInfo
	make_bitmapinfo (const ColorSpace& cs)
	{
		// Q: What color spaces does CGBitmapContextCreate support?
		// http://developer.apple.com/library/mac/#qa/qa1037/_index.html

		CGBitmapInfo info = 0;

		if (cs.is_alpha_first())
		{
			info |= cs.is_premult()
				? kCGImageAlphaPremultipliedFirst
				: kCGImageAlphaFirst;
		}
		else if (cs.is_alpha_last())
		{
			info |= cs.is_premult()
				? kCGImageAlphaPremultipliedLast
				: kCGImageAlphaLast;
		}
		else if (cs.is_skip_first())
			info |= kCGImageAlphaNoneSkipFirst;
		else if (cs.is_skip_last())
			info |= kCGImageAlphaNoneSkipLast;
		else
			info |= kCGImageAlphaNone;

		     if (cs.is_rgb()) info |= kCGBitmapByteOrder32Big;
		else if (cs.is_bgr()) info |= kCGBitmapByteOrder32Little;
		else return false;

		if (cs.is_float()) info |= kCGBitmapFloatComponents;

		return info;
	}


	struct Bitmap::Data
	{

		int width, height;

		ColorSpace color_space;

		void* pixels;

		CGContextRef context;

		bool dirty;

		Data ()
		:	pixels(NULL), context(NULL)
		{
			clear();
		}

		~Data ()
		{
			clear();
		}

		CGContextRef get_context ()
		{
			if (context) return context;

			int bpc   = color_space.bpc();
			int pitch = width * color_space.Bpp();
			if (bpc <= 0 || pitch <= 0) return NULL;

			CGColorSpaceRef cgcs = NULL;
			if (color_space.is_gray())
				cgcs = CGColorSpaceCreateDeviceGray();
			else if (color_space.is_rgb() || color_space.is_bgr())
				cgcs = CGColorSpaceCreateDeviceRGB();
			else
				return NULL;

			context = CGBitmapContextCreate(
				pixels, width, height, bpc, pitch, cgcs, make_bitmapinfo(color_space));
			CGColorSpaceRelease(cgcs);
			return context;
		}

		CGImageRef get_image ()
		{
			CGContextRef c = get_context();
			if (!c) return NULL;
			return CGBitmapContextCreateImage(c);
		}

		void clear ()
		{
			if (context) CGContextRelease(context);
			if (pixels) delete [] (uchar*) pixels;

			width = height = 0;
			color_space = COLORSPACE_UNKNOWN;
			pixels      = NULL;
			context     = NULL;
			dirty       = false;
		}

	};// Bitmap::Data


	static void
	setup_bitmap (
		Bitmap* this_,
		int w, int h, const ColorSpace& cs,
		const void* pixels_ = NULL, bool clear_pixels = true)
	{
		if (!this_ || w <= 0 || h <= 0 || !cs)
			argument_error(__FILE__, __LINE__);

		this_->self->clear();

		this_->self->width       = w;
		this_->self->height      = h;
		this_->self->color_space = cs;
		this_->self->dirty       = true;

		size_t size = w * h * cs.Bpp();
		this_->self->pixels = new uchar[size];

		if (pixels_)
			memcpy(this_->self->pixels, pixels_, size);
		else if (clear_pixels)
			memset(this_->self->pixels, 0, size);
	}

	static void
	setup_bitmap (Bitmap* this_, const Texture& tex)
	{
		if (!this_ || !tex)
			argument_error(__FILE__, __LINE__);

		setup_bitmap(this_, tex.width(), tex.height(), tex.color_space(), NULL, false);

		GLenum format, type;
		tex.color_space().get_gl_enums(&format, &type, tex.alpha_only());

		FrameBuffer fb(tex);
		FrameBufferBinder binder(fb.id());

		for (int y = 0; y < this_->height(); ++y)
			glReadPixels(0, y, this_->width(), 1, format, type, (GLvoid*) this_->at<uchar>(0, y));
	}


	Bitmap::Bitmap ()
	{
	}

	Bitmap::Bitmap (
		int width, int height, const ColorSpace& color_space, const void* pixels)
	{
		setup_bitmap(this, width, height, color_space, pixels);
	}

	Bitmap::Bitmap (const Texture& texture)
	{
		setup_bitmap(this, texture);
	}

	Bitmap::~Bitmap ()
	{
	}

	Bitmap
	Bitmap::copy () const
	{
		return Bitmap(width(), height(), color_space(), pixels());
	}

	int
	Bitmap::width () const
	{
		if (!*this) return 0;
		return self->width;
	}

	int
	Bitmap::height () const
	{
		if (!*this) return 0;
		return self->height;
	}

	const ColorSpace&
	Bitmap::color_space () const
	{
		if (!*this)
		{
			static const ColorSpace UNKNOWN = COLORSPACE_UNKNOWN;
			return UNKNOWN;
		}
		return self->color_space;
	}

	int
	Bitmap::pitch () const
	{
		return width() * self->color_space.Bpp();
	}

	size_t
	Bitmap::size () const
	{
		return pitch() * height();
	}

	void*
	Bitmap::pixels ()
	{
		if (!*this) return NULL;
		return self->pixels;
	}

	const void*
	Bitmap::pixels () const
	{
		return const_cast<This*>(this)->pixels();
	}

	bool
	Bitmap::dirty () const
	{
		return self->dirty;
	}

	void
	Bitmap::set_dirty (bool b)
	{
		self->dirty = b;
	}

	Bitmap::operator bool () const
	{
		return
			self->width > 0 && self->height > 0 && self->color_space && self->pixels;
	}

	bool
	Bitmap::operator ! () const
	{
		return !operator bool();
	}


	Bitmap
	load_bitmap (const char* path_)
	{
		if (!path_ || path_[0] == '\0')
			argument_error(__FILE__, __LINE__);

		NSString* path = [NSString stringWithUTF8String: path_];
		UIImage* uiimage = [UIImage imageWithContentsOfFile: path];
		if (!uiimage)
			rays_error(__FILE__, __LINE__, "[UIImage imageWithContentsOfFile:] failed.");

		CGImageRef image = [uiimage CGImage];
		if (!image)
			rays_error(__FILE__, __LINE__, "[imagerep CGImage] failed.");

		size_t width  = CGImageGetWidth(image);
		size_t height = CGImageGetHeight(image);

		Bitmap bmp((int) width, (int) height, RGBA);
		if (!bmp)
			rays_error(__FILE__, __LINE__, "invalid bitmap.");

		CGContextRef context = bmp.self->get_context();
		if (!context)
			rays_error(__FILE__, __LINE__, "creating CGContext failed.");

		CGContextDrawImage(context, CGRectMake(0, 0, width, height), image);
		return bmp;
	}

	void
	save_bitmap (const Bitmap& bmp, const char* path_)
	{
		boost::shared_ptr<CGImage> img(
			bmp.self->get_image(), CGImageRelease);
		if (!img)
			rays_error(__FILE__, __LINE__, "getting CGImage failed.");

		NSString* path = [NSString stringWithUTF8String: path_];
		NSURL* url = [NSURL fileURLWithPath: path];
		if (!url)
			rays_error(__FILE__, __LINE__, "creating NSURL failed.");

		boost::shared_ptr<CGImageDestination> dest(
			CGImageDestinationCreateWithURL((CFURLRef) url, kUTTypePNG, 1, NULL),
			safe_cfrelease);
		if (!dest)
			rays_error(__FILE__, __LINE__, "CGImageDestinationCreateWithURL() failed.");

		CGImageDestinationAddImage(dest.get(), img.get(), NULL);
		if (!CGImageDestinationFinalize(dest.get()))
			rays_error(__FILE__, __LINE__, "CGImageDestinationFinalize() failed.");
	}


	void draw_string (
		CGContextRef, coord, const char*, coord, coord, const Font&);

	void
	draw_string (
		Bitmap* bmp, const char* str, coord x, coord y, const Font& font)
	{
		if (!bmp || !*bmp || !str || !font)
			argument_error(__FILE__, __LINE__);

		if (*str == '\0') return;

		draw_string(bmp->self->get_context(), bmp->height(), str, x, y, font);
		bmp->set_dirty();
	}


}// Rays
