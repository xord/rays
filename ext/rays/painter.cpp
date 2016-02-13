#include "rays/ruby/painter.h"


#include <vector>
#include <rucy.h>
#include "rays/ruby/point.h"
#include "rays/ruby/bounds.h"
#include "rays/ruby/color.h"
#include "rays/ruby/matrix.h"
#include "rays/ruby/font.h"
#include "rays/ruby/image.h"
#include "rays/ruby/shader.h"
#include "defs.h"


using namespace Rucy;

using Rays::coord;


RUCY_DEFINE_VALUE_FROM_TO(Rays::Painter)

#define THIS  to<Rays::Painter*>(self)

#define CHECK RUCY_CHECK_OBJECT(Rays::Painter, self)


static
RUCY_DEF_ALLOC(alloc, klass)
{
	return new_type<Rays::Painter>(klass);
}
RUCY_END

static
RUCY_DEF4(canvas, x, y, width, height)
{
	CHECK;

	coord xx = to<coord>(x);
	coord yy = to<coord>(y);
	coord ww = to<coord>(width);
	coord hh = to<coord>(height);
	THIS->canvas(xx, yy, ww, hh);

	return self;
}
RUCY_END

static
RUCY_DEF0(bounds)
{
	CHECK;
	return value(THIS->bounds());
}
RUCY_END


static
RUCY_DEF0(begin_paint)
{
	CHECK;
	THIS->begin();
	return self;
}
RUCY_END

static
RUCY_DEF0(end_paint)
{
	CHECK;
	THIS->end();
	return self;
}
RUCY_END

static
RUCY_DEF0(clear)
{
	CHECK;
	THIS->clear();
}
RUCY_END

static
RUCY_DEFN(line)
{
	CHECK;
	check_arg_count(__FILE__, __LINE__, "Painter#line", argc, 2, 4);

	if (argc == 2)
		THIS->line(to<Rays::Point&>(argv[0]), to<Rays::Point&>(argv[1]));
	else
	{
		coord x1 = to<coord>(argv[0]);
		coord y1 = to<coord>(argv[1]);
		coord x2 = to<coord>(argv[2]);
		coord y2 = to<coord>(argv[3]);
		THIS->line(x1, y1, x2, y2);
	}

	return self;
}
RUCY_END

static
RUCY_DEFN(lines)
{
	CHECK;
	if (argc <= 0)
		argument_error(__FILE__, __LINE__, "Painter#lines");

	std::vector<Rays::Coord3> points;
	points.reserve(argc);
	for (int i = 0; i < argc; ++i)
		points.push_back(to<Rays::Point>(argv[i]));

	THIS->lines((Rays::Point*) &points[0], points.size());
	return self;
}
RUCY_END

static
RUCY_DEFN(polygon)
{
	CHECK;
	if (argc <= 0)
		argument_error(__FILE__, __LINE__, "Painter#polygon");

	std::vector<Rays::Coord3> points;
	points.reserve(argc);
	for (int i = 0; i < argc; ++i)
		points.push_back(to<Rays::Point&>(argv[i]));

	THIS->polygon((Rays::Point*) &points[0], points.size());
	return self;
}
RUCY_END

static
RUCY_DEFN(rect)
{
	CHECK;
	check_arg_count(
		__FILE__, __LINE__, "Painter#rect", argc, 1, 2, 4, 5, 6, 8, 9);

	if (argv[0].is_kind_of(Rays::bounds_class()))
	{
		Rays::Bounds& b = to<Rays::Bounds&>(argv[0]);
		coord lt        = argc >= 2 ? to<coord>(argv[1]) : 0;
		coord rt        = argc >= 3 ? to<coord>(argv[2]) : lt;
		coord lb        = argc >= 4 ? to<coord>(argv[3]) : lt;
		coord rb        = argc >= 5 ? to<coord>(argv[4]) : lt;
		uint nsegment   = argc >= 6 ? to<uint>(argv[5])  : 0;
		THIS->rect(b, lt, rt, lb, rb, nsegment);
	}
	else
	{
		coord x       = to<coord>(argv[0]);
		coord y       = to<coord>(argv[1]);
		coord w       = to<coord>(argv[2]);
		coord h       = to<coord>(argv[3]);
		coord lt      = argc >= 5 ? to<coord>(argv[4]) : 0;
		coord rt      = argc >= 6 ? to<coord>(argv[5]) : lt;
		coord lb      = argc >= 7 ? to<coord>(argv[6]) : lt;
		coord rb      = argc >= 8 ? to<coord>(argv[7]) : lt;
		uint nsegment = argc >= 9 ? to<uint>(argv[8])  : 0;
		THIS->rect(x, y, w, h, lt, rt, lb, rb, nsegment);
	}

	return self;
}
RUCY_END

static
RUCY_DEFN(ellipse)
{
	CHECK;

	if (argv[0].is_kind_of(Rays::bounds_class()))
	{
		check_arg_count(
			__FILE__, __LINE__, "Painter#ellipse(Bounds, ...)", argc, 1, 2, 3, 4, 5);

		const Rays::Bounds& b = to<Rays::Bounds&>(argv[0]);
		float from = (argc >= 2) ? to<float>(argv[1]) : 0;
		float to_  = (argc >= 3) ? to<float>(argv[2]) : 360;
		coord min_ = (argc >= 4) ? to<coord>(argv[3]) : 0;
		uint nseg  = (argc >= 5) ? to<uint> (argv[4]) : 0;
		THIS->ellipse(b, from, to_, min_, nseg);
	}
	else if (argv[0].is_kind_of(Rays::point_class()))
	{
		check_arg_count(
			__FILE__, __LINE__, "Painter#ellipse(Point, ...)", argc, 2, 3, 4, 5, 6);

		const Rays::Point& p = to<Rays::Point&>(argv[0]);
		coord radius = to<coord>(argv[1]);
		float from   = (argc >= 3) ? to<float>(argv[2]) : 0;
		float to_    = (argc >= 4) ? to<float>(argv[3]) : 360;
		coord min_   = (argc >= 5) ? to<coord>(argv[4]) : 0;
		uint nseg    = (argc >= 6) ? to<uint> (argv[5]) : 0;
		THIS->ellipse(p, radius, from, to_, min_, nseg);
	}
	else
	{
		check_arg_count(
			__FILE__, __LINE__,
			"Painter#ellipse(Number, ...)", argc, 3, 4, 5, 6, 7, 8);

		coord x    = to<coord>(argv[0]);
		coord y    = to<coord>(argv[1]);
		coord w    = to<coord>(argv[2]);
		coord h    = (argc >= 4) ? to<coord>(argv[3]) : w;
		float from = (argc >= 5) ? to<float>(argv[4]) : 0;
		float to_  = (argc >= 6) ? to<float>(argv[5]) : 360;
		coord min_ = (argc >= 7) ? to<coord>(argv[6]) : 0;
		uint nseg  = (argc >= 8) ? to<uint> (argv[7]) : 0;
		THIS->ellipse(x, y, w, h, from, to_, min_, nseg);
	}

	return self;
}
RUCY_END

static
RUCY_DEFN(image)
{
	CHECK;
	check_arg_count(__FILE__, __LINE__, "Painter#image", argc, 1, 3, 5, 7, 9);

	const Rays::Image* image = to<Rays::Image*>(argv[0]);
	if (!image)
		argument_error(__FILE__, __LINE__, "%s is not an image.", argv[0].inspect().c_str());

	if (argc == 1)
		THIS->image(*image);
	else if (argc == 3)
	{
		coord x = to<coord>(argv[1]), y = to<coord>(argv[2]);
		THIS->image(*image, x, y);
	}
	else if (argc == 5)
	{
		coord x = to<coord>(argv[1]), w = to<coord>(argv[3]);
		coord y = to<coord>(argv[2]), h = to<coord>(argv[4]);
		THIS->image(*image, x, y, w, h);
	}
	else if (argc == 7)
	{
		coord sx = to<coord>(argv[1]), dx = to<coord>(argv[5]);
		coord sy = to<coord>(argv[2]), dy = to<coord>(argv[6]);
		coord sw = to<coord>(argv[3]);
		coord sh = to<coord>(argv[4]);
		THIS->image(*image, sx, sy, sw, sh, dx, dy);
	}
	else if (argc == 9)
	{
		coord sx = to<coord>(argv[1]), dx = to<coord>(argv[5]);
		coord sy = to<coord>(argv[2]), dy = to<coord>(argv[6]);
		coord sw = to<coord>(argv[3]), dw = to<coord>(argv[7]);
		coord sh = to<coord>(argv[4]), dh = to<coord>(argv[8]);
		THIS->image(*image, sx, sy, sw, sh, dx, dy, dw, dh);
	}

	return self;
}
RUCY_END

static
RUCY_DEFN(text)
{
	CHECK;
	check_arg_count(__FILE__, __LINE__, "Painter#text", argc, 1, 3, 4, 5, 7);

	if (argc == 1)
		THIS->text(argv[0].c_str());
	else if (argc == 3)
	{
		coord x = to<coord>(argv[1]), y = to<coord>(argv[2]);
		THIS->text(argv[0].c_str(), x, y);
	}
	else if (argc == 4)
	{
		const Rays::Font* font = to<Rays::Font*>(argv[3]);
		if (!font || !*font)
			rays_error(__FILE__, __LINE__, "Painter#text: invalid font.");

		coord x = to<coord>(argv[1]), y = to<coord>(argv[2]);
		THIS->text(argv[0].c_str(), x, y, font);
	}
	else if (argc == 5)
	{
		coord x = to<coord>(argv[1]), w = to<coord>(argv[3]);
		coord y = to<coord>(argv[2]), h = to<coord>(argv[4]);
		THIS->text(argv[0].c_str(), x, y, w, h);
	}
	else if (argc == 7)
	{
		const Rays::Font* font = to<Rays::Font*>(argv[3]);
		if (!font || !*font)
			rays_error(__FILE__, __LINE__, "Painter#text: invalid font.");

		coord x = to<coord>(argv[1]), w = to<coord>(argv[3]);
		coord y = to<coord>(argv[2]), h = to<coord>(argv[4]);
		THIS->text(argv[0].c_str(), x, y, w, h, font);
	}

	return self;
}
RUCY_END

static
RUCY_DEFN(set_background)
{
	CHECK;
	THIS->set_background(to<Rays::Color>(argc, argv));
	return self;
}
RUCY_END

static
RUCY_DEF0(get_background)
{
	CHECK;
	return value(THIS->background());
}
RUCY_END

static
RUCY_DEF0(no_background)
{
	CHECK;
	THIS->no_background();
	return self;
}
RUCY_END

static
RUCY_DEFN(set_fill)
{
	CHECK;
	THIS->set_fill(to<Rays::Color>(argc, argv));
	return self;
}
RUCY_END

static
RUCY_DEF0(get_fill)
{
	CHECK;
	return value(THIS->fill());
}
RUCY_END

static
RUCY_DEF0(no_fill)
{
	CHECK;
	THIS->no_fill();
	return self;
}
RUCY_END

static
RUCY_DEFN(set_stroke)
{
	CHECK;
	THIS->set_stroke(to<Rays::Color>(argc, argv));
	return self;
}
RUCY_END

static
RUCY_DEF0(get_stroke)
{
	CHECK;
	return value(THIS->stroke());
}
RUCY_END

static
RUCY_DEF0(no_stroke)
{
	CHECK;
	THIS->no_stroke();
	return self;
}
RUCY_END

static
RUCY_DEFN(set_clip)
{
	CHECK;
	THIS->set_clip(to<Rays::Bounds>(argc, argv));
	return self;
}
RUCY_END

static
RUCY_DEF0(get_clip)
{
	CHECK;
	return value(THIS->clip());
}
RUCY_END

static
RUCY_DEF0(no_clip)
{
	CHECK;
	THIS->no_clip();
	return self;
}
RUCY_END

static
RUCY_DEFN(set_font)
{
	CHECK;
	check_arg_count(__FILE__, __LINE__, "Painter#set_font", argc, 0, 1, 2);

	THIS->set_font(to<Rays::Font>(argc, argv));
	return self;
}
RUCY_END

static
RUCY_DEF0(get_font)
{
	CHECK;
	return value(THIS->font());
}
RUCY_END

static
RUCY_DEF0(push_attrs)
{
	CHECK;
	THIS->push_attrs();
	return self;
}
RUCY_END

static
RUCY_DEF0(pop_attrs)
{
	CHECK;
	THIS->pop_attrs();
	return self;
}
RUCY_END


static
RUCY_DEF1(attach_shader, shader)
{
	CHECK;
	Rays::Shader t = to<Rays::Shader>(shader);
	THIS->attach(t);
	return value(t);
}
RUCY_END

static
RUCY_DEF1(detach_shader, shader)
{
	CHECK;
	THIS->detach(to<Rays::Shader&>(shader));
	return self;
}
RUCY_END

static
RUCY_DEFN(set_uniform)
{
	CHECK;
	check_arg_count(__FILE__, __LINE__, "Painter#set_uniform", argc, 2, 3, 4, 5);

	#define Ai(n) (argv[n].as_i())
	#define Af(n) ((float) argv[n].as_f())

	const char* name = argv[0].c_str();
	if (argv[1].is_i())
	{
		switch (argc)
		{
			case 2: THIS->set_uniform(name, Ai(1)); break;
			case 3: THIS->set_uniform(name, Ai(1), Ai(2)); break;
			case 4: THIS->set_uniform(name, Ai(1), Ai(2), Ai(3)); break;
			case 5: THIS->set_uniform(name, Ai(1), Ai(2), Ai(3), Ai(4)); break;
		}
	}
	else if (argv[1].is_f())
	{
		switch (argc)
		{
			case 2: THIS->set_uniform(name, Af(1)); break;
			case 3: THIS->set_uniform(name, Af(1), Af(2)); break;
			case 4: THIS->set_uniform(name, Af(1), Af(2), Af(3)); break;
			case 5: THIS->set_uniform(name, Af(1), Af(2), Af(3), Af(4)); break;
		}
	}
	else
		argument_error(__FILE__, __LINE__);

	#undef Ai
	#undef Af

	return self;
}
RUCY_END

static
RUCY_DEF0(push_shaders)
{
	CHECK;
	THIS->push_shaders();
	return self;
}
RUCY_END

static
RUCY_DEF0(pop_shaders)
{
	CHECK;
	THIS->pop_shaders();
	return self;
}
RUCY_END


static
RUCY_DEFN(translate)
{
	CHECK;
	check_arg_count(__FILE__, __LINE__, "Painter#translate", argc, 2, 3);

	coord xx = to<coord>(argv[0]);
	coord yy = to<coord>(argv[1]);
	coord zz = (argc >= 3) ? to<coord>(argv[2]) : 0;
	THIS->translate(xx, yy, zz);

	return self;
}
RUCY_END

static
RUCY_DEFN(scale)
{
	CHECK;
	check_arg_count(__FILE__, __LINE__, "Painter#scale", argc, 2, 3);

	coord xx = to<coord>(argv[0]);
	coord yy = to<coord>(argv[1]);
	coord zz = (argc >= 3) ? to<coord>(argv[2]) : 1;
	THIS->scale(xx, yy, zz);

	return self;
}
RUCY_END

static
RUCY_DEFN(rotate)
{
	CHECK;
	check_arg_count(__FILE__, __LINE__, "Painter#rotate", argc, 1, 3, 4);

	coord aa = to<coord>(argv[0]), xx = 0, yy = 0, zz = 1;
	if (argc >= 3)
	{
		xx = to<coord>(argv[1]);
		yy = to<coord>(argv[2]);
		zz = argc >= 4 ? to<coord>(argv[3]) : 0;
	}

	THIS->rotate(aa, xx, yy, zz);

	return self;
}
RUCY_END

static
RUCY_DEFN(set_matrix)
{
	CHECK;
	THIS->set_matrix(to<Rays::Matrix>(argc, argv));
	return value(THIS->matrix());
}
RUCY_END

static
RUCY_DEF0(get_matrix)
{
	CHECK;
	return value(THIS->matrix());
}
RUCY_END

static
RUCY_DEF0(push_matrix)
{
	CHECK;
	THIS->push_matrix();
	return self;
}
RUCY_END

static
RUCY_DEF0(pop_matrix)
{
	CHECK;
	THIS->pop_matrix();
	return self;
}
RUCY_END


static Class cPainter;

void
Init_painter ()
{
	Module mRays = define_module("Rays");

	cPainter = mRays.define_class("Painter");
	cPainter.define_alloc_func(alloc);

	cPainter.define_method("canvas", canvas);
	cPainter.define_method("bounds", bounds);

	cPainter.define_private_method("begin_paint", begin_paint);
	cPainter.define_private_method(  "end_paint",   end_paint);
	cPainter.define_method("clear",   clear);
	cPainter.define_method("line",    line);
	cPainter.define_method("lines",   lines);
	cPainter.define_method("polygon", polygon);
	cPainter.define_method("rect",    rect);
	cPainter.define_method("ellipse", ellipse);
	cPainter.define_method("image",   image);
	cPainter.define_method("text",    text);

	cPainter.define_method(   "background=", set_background);
	cPainter.define_method(   "background",  get_background);
	cPainter.define_method("no_background",   no_background);
	cPainter.define_method(   "fill=", set_fill);
	cPainter.define_method(   "fill",  get_fill);
	cPainter.define_method("no_fill",   no_fill);
	cPainter.define_method(   "stroke=", set_stroke);
	cPainter.define_method(   "stroke",  get_stroke);
	cPainter.define_method("no_stroke",   no_stroke);
	cPainter.define_method(   "clip=", set_clip);
	cPainter.define_method(   "clip",  get_clip);
	cPainter.define_method("no_clip",   no_clip);
	cPainter.define_method("font=", set_font);
	cPainter.define_method("font",  get_font);
	cPainter.define_method("push_attrs", push_attrs);
	cPainter.define_method( "pop_attrs",  pop_attrs);

	cPainter.define_private_method("attach_shader", attach_shader);
	cPainter.define_private_method("detach_shader", detach_shader);
	cPainter.define_private_method("set_uniform", set_uniform);
	cPainter.define_method("push_shaders", push_shaders);
	cPainter.define_method( "pop_shaders",  pop_shaders);

	cPainter.define_method("translate", translate);
	cPainter.define_method("scale",     scale);
	cPainter.define_method("rotate",    rotate);
	cPainter.define_private_method("matrix=", set_matrix);
	cPainter.define_private_method("matrix",  get_matrix);
	cPainter.define_method("push_matrix", push_matrix);
	cPainter.define_method( "pop_matrix",  pop_matrix);
}


namespace Rays
{


	Class
	painter_class ()
	{
		return cPainter;
	}


}// Rays
