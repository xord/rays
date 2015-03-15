#include "rays/font.h"


#include "gdi.h"


namespace Rays
{


	struct Font::Data
	{

		Win32::Font font;

	};// Window::Data


	Font::Font ()
	{
	}

	Font::Font (const char* name, coord size)
	{
		self->font = Win32::Font(name, size);
	}

	Font::~Font ()
	{
	}

	String
	Font::name () const
	{
		return self->font.name();
	}

	coord
	Font::size () const
	{
		return self->font.size();
	}

	bool
	Font::get_extent (coord* width, coord* height, const char* str) const
	{
		return self->font.get_extent(width, height, str);
	}

	Font::operator bool () const
	{
		return self && self->font;
	}

	bool
	Font::operator ! () const
	{
		return !operator bool();
	}


	const Font&
	default_font ()
	{
		static const Font FONT(NULL);
		return FONT;
	}


	bool
	draw_string (
		HDC hdc, coord context_height,
		const char* str, coord x, coord y, const Font& font)
	{
		using namespace Win32;

		if (!hdc || !str || !font) return false;

		if (*str == '\0') return true;

		coord width = 0, height = 0;
		if (!font.get_extent(&width, &height, str))
			return false;

		DC dc = hdc;

		RECT rect = {x, y, x + (int) width, y + (int) height};
		FillRect(dc.handle(), &rect, Brush(0, 0, 0).handle());

		Win32::Font old = dc.font();
		dc.set_font(font.self->font.handle());
		BOOL ret = TextOutA(dc.handle(), x, y, str, strlen(str));
		dc.set_font(old);

		return ret != FALSE;
	}


}// Rays
