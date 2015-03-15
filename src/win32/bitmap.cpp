#include "rays/bitmap.h"


#include "gdi.h"


namespace Rays
{


	struct Bitmap::Data
	{

		int width, height, pitch;

		ColorSpace colorspace;

		void* pixels;

		Win32::MemoryDC memdc;

		bool dirty;

		Data ()
		:	pixels(NULL), dirty(true)
		{
		}

	};// Window::Data


	static bool
	init_bitmap_pixels (Bitmap* bmp)
	{
		if (!*bmp) return false;

		memset(bmp->data(), 0, bmp->size());

		if (!bmp->color_space().has_alpha())
			return true;

		int Bpp = bmp->color_space().Bpp();
		for (int y = 0; y < bmp->height(); ++y)
		{
			unsigned char* p =
				bmp->at<unsigned char>(0, y) + bmp->color_space().alpha_pos();
			int w = bmp->width();
			while (w--)
			{
				*p = 255;
				p += Bpp;
			}
		}

		return true;
	}

	static bool
	setup (Bitmap* bmp, int w, int h, const ColorSpace& cs, HDC hdc = NULL)
	{
		if (w <= 0 || h <= 0 || !cs || !bmp || *bmp)
			return false;

		Bitmap::Data* self = bmp->self.get();

		self->width      = w;
		self->height     = h;
		self->colorspace = cs;
		self->pitch      = self->width * self->colorspace.Bpp();

		int padding = 4 - self->pitch % 4;
		if (padding < 4) self->pitch += padding;

		BITMAPINFO bmpinfo;
		memset(&bmpinfo, 0, sizeof(bmpinfo));

		BITMAPINFOHEADER& header = bmpinfo.bmiHeader;
		header.biSize        = sizeof(BITMAPINFOHEADER);
		header.biWidth       = self->width;
		header.biHeight      = -self->height;
		header.biPlanes      = 1;
		header.biBitCount    = self->colorspace.bpp();
		header.biCompression = BI_RGB;

		Win32::DC dc = hdc ? Win32::DC(hdc) : Win32::screen_dc();

		HBITMAP hbmp = CreateDIBSection(
			dc.handle(), &bmpinfo, DIB_RGB_COLORS, (void**) &self->pixels, NULL, 0);
		if (!hbmp) return false;

		self->memdc = Win32::MemoryDC(dc.handle(), Win32::Bitmap(hbmp, true));
		if (!self->memdc) return false;

		return init_bitmap_pixels(bmp);
	}


	Bitmap::Bitmap ()
	{
	}

	Bitmap::Bitmap (int width, int height, const ColorSpace& cs)
	{
		setup(this, width, height, cs);
	}

	Bitmap::~Bitmap ()
	{
	}

	int
	Bitmap::width () const
	{
		return self->width;
	}

	int
	Bitmap::height () const
	{
		return self->height;
	}

	const ColorSpace&
	Bitmap::color_space () const
	{
		return self->colorspace;
	}

	int
	Bitmap::pitch () const
	{
		return self->pitch;
	}

	size_t
	Bitmap::size () const
	{
		return pitch() * height();
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

	void*
	Bitmap::data ()
	{
		return self->pixels;
	}

	const void*
	Bitmap::data () const
	{
		return const_cast<This*>(this)->data();
	}

	Bitmap::operator bool () const
	{
		return
			self &&
			self->width > 0 && self->height > 0 && self->pitch > 0 &&
			self->colorspace && self->pixels && self->memdc;
	}

	bool
	Bitmap::operator ! () const
	{
		return !operator bool();
	}


	bool
	load_bitmap (Bitmap* bitmap, const char* path)
	{
		return false;
	}

	bool
	save_bitmap (const Bitmap& bitmap, const char* path)
	{
		return false;
	}


	bool draw_string (
		HDC, coord, const char*, coord, coord, const Font&);

	bool
	draw_string (
		Bitmap* bmp, const char* str, coord x, coord y, const Font& font)
	{
		if (!bmp || !*bmp || !str || !font) return false;

		if (*str == '\0') return true;

		if (!draw_string(bmp->self->memdc.handle(), bmp->height(), str, x, y, font))
			return false;

		bmp->set_dirty();
		return true;
	}


}// Rays
