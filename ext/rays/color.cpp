#include "rays/ruby/color.h"


#include <map>
#include <rucy.h>
#include "defs.h"


using namespace Rucy;


RUCY_DEFINE_VALUE_OR_ARRAY_FROM_TO(Rays::Color)

#define THIS  to<Rays::Color*>(self)

#define CHECK RUCY_CHECK_OBJ(Rays::Color, self)


static
RUCY_DEF_ALLOC(alloc, klass)
{
	return new_type<Rays::Color>(klass);
}
RUCY_END

static
RUCY_DEFN(initialize)
{
	CHECK;
	check_arg_count(__FILE__, __LINE__, "Color#initialize", argc, 0, 1, 2, 3, 4);

	if (argc >= 1)
		*THIS = to<Rays::Color>(argc, argv);

	return self;
}
RUCY_END

static
RUCY_DEF1(initialize_copy, obj)
{
	CHECK;
	*THIS = to<Rays::Color&>(obj);
	return self;
}
RUCY_END

static
RUCY_DEF1(set_red, red)
{
	CHECK;

	return value(THIS->red = to<float>(red));
}
RUCY_END

static
RUCY_DEF0(get_red)
{
	CHECK;

	return value(THIS->red);
}
RUCY_END

static
RUCY_DEF1(set_green, green)
{
	CHECK;

	return value(THIS->green = to<float>(green));
}
RUCY_END

static
RUCY_DEF0(get_green)
{
	CHECK;

	return value(THIS->green);
}
RUCY_END

static
RUCY_DEF1(set_blue, blue)
{
	CHECK;

	return value(THIS->blue = to<float>(blue));
}
RUCY_END

static
RUCY_DEF0(get_blue)
{
	CHECK;

	return value(THIS->blue);
}
RUCY_END

static
RUCY_DEF1(set_alpha, alpha)
{
	CHECK;

	return value(THIS->alpha = to<float>(alpha));
}
RUCY_END

static
RUCY_DEF0(get_alpha)
{
	CHECK;

	return value(THIS->alpha);
}
RUCY_END


static Class cColor;

void
Init_color ()
{
	Module mRays = define_module("Rays");

	cColor = mRays.define_class("Color");
	cColor.define_alloc_func(alloc);
	cColor.define_private_method("initialize",      initialize);
	cColor.define_private_method("initialize_copy", initialize_copy);
	cColor.define_method("red=", set_red);
	cColor.define_method("red", get_red);
	cColor.define_method("green=", set_green);
	cColor.define_method("green", get_green);
	cColor.define_method("blue=", set_blue);
	cColor.define_method("blue", get_blue);
	cColor.define_method("alpha=", set_alpha);
	cColor.define_method("alpha", get_alpha);
}


namespace Rucy
{


	static int
	char2hex (char c)
	{
		     if ('0' <= c && c <= '9') return      c - '0';
		else if ('a' <= c && c <= 'f') return 10 + c - 'a';
		else if ('A' <= c && c <= 'F') return 10 + c - 'A';
		else                           return -1;
	}

	static bool
	parse_channel (float* channel, const char* str, size_t nchars, size_t index)
	{
		assert(channel && str && 1 <= nchars && nchars <= 2 && 0 <= index && index <= 3);

		const char* p = str + index * nchars;

		if (nchars == 1)
		{
			int c0 = char2hex(p[0]);
			if (c0 < 0)
				return false;

			assert(c0 < 16);

			*channel = c0 / 15.f;
			return true;
		}
		else
		{
			int c0 = char2hex(p[0]);
			int c1 = char2hex(p[1]);
			if (c0 < 0 || c1 < 0)
				return false;

			assert(c0 < 16 && c1 < 16);

			*channel = (c0 * 16 + c1) / 255.f;
			return true;
		}
	}

	static bool
	parse_string (Rays::Color* color, const char* str)
	{
		assert(color && str);

		if (*str != '#') return false;
		++str;

		size_t len = strlen(str);
		switch (len)
		{
			case 3:
			case 6:
			{
				size_t nchars = len / 3;

				float r, g, b;
				if (
					!parse_channel(&r, str, nchars, 0) ||
					!parse_channel(&g, str, nchars, 1) ||
					!parse_channel(&b, str, nchars, 2))
				{
					return false;
				}

				color->reset(r, g, b);
				break;
			}

			case 4:
			case 8:
			{
				size_t nchars = len / 4;

				float r, g, b, a;
				if (
					!parse_channel(&r, str, nchars, 0) ||
					!parse_channel(&g, str, nchars, 1) ||
					!parse_channel(&b, str, nchars, 2) ||
					!parse_channel(&a, str, nchars, 3))
				{
					return false;
				}

				color->reset(r, g, b, a);
				break;
			}

			default: return false;
		}

		return true;
	}

	static const Rays::Color&
	find_color (const char* name)
	{
		typedef std::map<Rays::String, Rays::Color> ColorMap;

		static ColorMap colors;
		if (colors.empty())
		{
			colors["no"]          =
			colors["none"]        =
			colors["transp"]      =
			colors["transparent"] = Rays::Color(0, 0);

			colors["black"]   = Rays::Color(0);
			colors["gray"]    = Rays::Color(0.5);
			colors["white"]   = Rays::Color(1);
			colors["red"]     = Rays::Color(1, 0, 0);
			colors["green"]   = Rays::Color(0, 1, 0);
			colors["blue"]    = Rays::Color(0, 0, 1);
			colors["yellow"]  = Rays::Color(1, 1, 0);
			colors["cyan"]    = Rays::Color(0, 1, 1);
			colors["magenta"] = Rays::Color(1, 0, 1);
		}

		assert(name);

		ColorMap::const_iterator it = colors.find(Rays::String(name).downcase());
		if (it == colors.end())
			argument_error(__FILE__, __LINE__, "color '%s' is not found.", name);

		return it->second;
	}

	static Rays::Color
	str2color (const char* str)
	{
		if (!str)
			argument_error(__FILE__, __LINE__);

		Rays::String str_ = str;
		str_ = str_.strip();

		Rays::Color color;
		if (parse_string(&color, str_.c_str()))
			return color;
		else
			return find_color(str_.c_str());
	}

	template <> Rays::Color
	value_to<Rays::Color> (int argc, const Value*argv, bool convert)
	{
		if (argc == 1 && argv->is_array())
		{
			argc = argv->size();
			argv = argv->as_array();
		}

		assert(argc == 0 || (argc > 0 && argv));

		if (convert)
		{
			if (argc == 0)
				return Rays::Color();
			else if (argv->is_nil())
				return str2color("none");
			else if (argv->is_s() || argv->is_sym())
				return str2color(argv[0].c_str());
			else if (argv->is_i() || argv->is_f())
			{
				switch (argc)
				{
					#define V(i) argv[i].as_f(true)
					case 1: return Rays::Color(V(0));
					case 2: return Rays::Color(V(0), V(1));
					case 3: return Rays::Color(V(0), V(1), V(2));
					case 4: return Rays::Color(V(0), V(1), V(2), V(3));
					#undef V
					default: argument_error(__FILE__, __LINE__, "invalid array size.");
				}
			}
		}

		if (argc != 1)
			argument_error(__FILE__, __LINE__);

		return value_to<Rays::Color&>(*argv, convert);
	}


}// Rucy


namespace Rays
{


	Class
	color_class ()
	{
		return cColor;
	}


}// Rays
