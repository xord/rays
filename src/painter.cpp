#include "rays/painter.h"


#include <math.h>
#include <list>
#include <algorithm>
#include <boost/scoped_array.hpp>
#include "rays/exception.h"
#include "rays/point.h"
#include "rays/bounds.h"
#include "rays/color.h"
#include "rays/matrix.h"
#include "rays/font.h"
#include "rays/bitmap.h"
#include "rays/texture.h"
#include "rays/image.h"
#include "rays/shader.h"
#include "frame_buffer.h"
#include "program.h"


namespace Rays
{


	enum ColorType
	{

		FILL = 0,
		STROKE,

		COLOR_TYPE_END,
		COLOR_TYPE_BEGIN = 0

	};// ColorType


	static const float PI      = M_PI;

	static const float PI_2    = PI * 2;

	static const float PI_HALF = PI / 2;


	struct Attributes
	{

		Bounds clip;

		Color background, colors[2];

		Font font;

		void init ()
		{
			clip           .reset(-1);
			background     .reset(0, 0);
			colors[FILL]   .reset(1, 1);
			colors[STROKE] .reset(1, 0);
			font           = default_font();
		}

	};// Attributes


	struct Painter::Data
	{

		Bounds viewport;

		Attributes attrs;

		Program program;

		std::list<Attributes> attrs_stack;

		bool painting;

		GLenum prev_matrix_mode;

		GLuint current_texture;

		Image text_image;

		FrameBuffer frame_buffer;

		float scale_factor;

		mutable Matrix matrix_tmp;

		Data ()
		:	painting(false), prev_matrix_mode(0), current_texture(0),
			text_image(1, 1, GRAY, true), scale_factor(1)
		{
			attrs.init();
		}

		void update_clip ()
		{
			if (attrs.clip)
			{
				glEnable(GL_SCISSOR_TEST);
				glScissor(
					attrs.clip.x,
					viewport.height - attrs.clip.height - attrs.clip.y,
					attrs.clip.width,
					attrs.clip.height);
			}
			else
			{
				glDisable(GL_SCISSOR_TEST);
			}

			check_error(__FILE__, __LINE__);
		}

		bool use_color (ColorType type)
		{
			const Color& c = attrs.colors[type];
			if (c.alpha <= 0) return false;

			glColor4f(c.red, c.green, c.blue, c.alpha);
			return true;
		}

		bool has_color ()
		{
			return
				attrs.colors[FILL]  .alpha > 0 ||
				attrs.colors[STROKE].alpha > 0;
		}

		void draw_shape (
			GLenum mode,
			int nindices, const uint* indices,
			int vertex_size, int vertex_stride, const coord* vertices,
			const coord* tex_coords = NULL, const Texture* texture = NULL)
		{
			if (nindices <= 0 || !indices || !vertices)
				argument_error(__FILE__, __LINE__);

			if (!painting)
				invalid_state_error(__FILE__, __LINE__, "'painting' should be true.");

			bool use_texture = texture && tex_coords;

			if (use_texture)
			{
				if (!*texture)
					argument_error(__FILE__, __LINE__, "invalid texture.");

				GLuint id = texture->id();
				if (id != current_texture)
				{
					glBindTexture(GL_TEXTURE_2D, id);
					current_texture = id;
				}

				glEnable(GL_TEXTURE_2D);

				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glTexCoordPointer(2, GL_FLOAT, 0, tex_coords);
			}
			else
			{
				glDisable(GL_TEXTURE_2D);
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			}

			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(vertex_size, GL_FLOAT, vertex_stride, vertices);

			glDrawElements(mode, nindices, GL_UNSIGNED_INT, indices);

			glDisableClientState(GL_VERTEX_ARRAY);
			if (use_texture)
			{
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				glDisable(GL_TEXTURE_2D);
			}
		}

	};// Painter::Data


	void
	set_painter_scale_factor (Painter* painter, float factor)
	{
		assert(painter);

		painter->self->scale_factor = factor;
	}


	Painter::Painter ()
	{
	}

	Painter::~Painter ()
	{
	}

	void
	Painter::canvas (coord x, coord y, coord width, coord height)
	{
		canvas(Bounds(x, y, -100, width, height, 200));
	}

	void
	Painter::canvas (coord x, coord y, coord z, coord width, coord height, coord depth)
	{
		canvas(Bounds(x, y, z, width, height, depth));
	}

	void
	Painter::canvas (const Bounds& bounds)
	{
		if (!bounds)
			argument_error(__FILE__, __LINE__);

		if (self->painting)
			invalid_state_error(__FILE__, __LINE__, "self->painting should be false.");

		self->viewport = bounds;
	}

	void
	Painter::bind (const Texture& texture)
	{
		if (!texture)
			argument_error(__FILE__, __LINE__, "invalid texture.");

		if (self->painting)
			invalid_state_error(__FILE__, __LINE__, "self->painting should be false.");

		FrameBuffer fb(texture);
		if (!fb)
			rays_error(__FILE__, __LINE__, "invalid frame buffer.");

		unbind();

		self->frame_buffer = fb;
		canvas(0, 0, fb.width(), fb.height());
	}

	void
	Painter::unbind ()
	{
		if (self->painting)
			invalid_state_error(__FILE__, __LINE__, "self->painting should be true.");

		self->frame_buffer = FrameBuffer();
	}

	const Bounds&
	Painter::bounds () const
	{
		return self->viewport;
	}


	void
	Painter::begin ()
	{
		if (self->painting)
			invalid_state_error(__FILE__, __LINE__, "self->painting should be false.");

		FrameBuffer& fb = self->frame_buffer;
		if (fb) bind_frame_buffer(fb.id());

		push_attrs();
		push_shaders();

		const Bounds& vp = self->viewport;
		float scale      = self->scale_factor;
		glViewport(
			(int) vp.x,             (int) vp.y,
			(int) vp.width * scale, (int) vp.height * scale);

		coord x1 = vp.x, x2 = vp.x + vp.width;
		coord y1 = vp.y, y2 = vp.y + vp.height;
		coord z1 = vp.z, z2 = vp.z + vp.depth;
		if (z1 == 0 && z2 == 0) {z1 = -100; z2 = 200;}
		if (!fb)                std::swap(y1, y2);

		glGetIntegerv(GL_MATRIX_MODE, (GLint*) &self->prev_matrix_mode);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(x1, x2, y1, y2, z1, z2);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glTranslatef(0.375f, 0.375f, 0);

		glMatrixMode(GL_TEXTURE);
		glPushMatrix();
		glLoadIdentity();

	#ifndef IOS
		glMatrixMode(GL_COLOR);
		glPushMatrix();
		glLoadIdentity();
	#endif

		glMatrixMode(GL_MODELVIEW);

		//glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		check_error(__FILE__, __LINE__);

		self->painting = true;

		no_clip();
	}

	void
	Painter::end ()
	{
		if (!self->painting)
			invalid_state_error(__FILE__, __LINE__, "self->painting should be true.");

		self->painting = false;

		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

		glMatrixMode(GL_TEXTURE);
		glPopMatrix();

	#ifndef IOS
		glMatrixMode(GL_COLOR);
		glPopMatrix();
	#endif

		glMatrixMode(self->prev_matrix_mode);

		pop_shaders();
		pop_attrs();

		//glFinish();

		if (self->frame_buffer)
		{
			unbind_frame_buffer();

			Texture& tex = self->frame_buffer.texture();
			if (tex) tex.set_dirty(true);
		}
	}

	void
	Painter::clear ()
	{
		if (!self->painting)
			invalid_state_error(__FILE__, __LINE__, "self->painting should be true.");

		const Color& c = self->attrs.background;
		glClearColor(c.red, c.green, c.blue, c.alpha);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		check_error(__FILE__, __LINE__);
	}

	void
	Painter::line (coord x1, coord y1, coord x2, coord y2)
	{
		static const uint INDICES[] =
		{
			0, 1
		};

		Data* pself = self.get();

		if (!pself->painting)
			invalid_state_error(__FILE__, __LINE__, "self->painting should be true.");

		if (!pself->use_color(STROKE))
			return;

		coord vertices[] =
		{
			x1, y1,
			x2, y2
		};

		pself->draw_shape(GL_LINES, 2, INDICES, 2, 0, vertices);
	}

	void
	Painter::line (const Point& p1, const Point& p2)
	{
		line(p1.x, p1.y, p2.x, p2.y);
	}

	void
	draw_lines (
		Painter* painter, const coord* points, size_t npoints,
		size_t vertex_size, size_t vertex_stride = 0)
	{
		assert(painter);

		Painter::Data* self = painter->self.get();

		if (!self->painting)
			invalid_state_error(__FILE__, __LINE__, "self->painting should be true.");

		if (!self->use_color(STROKE))
			return;

		boost::scoped_array<uint> indices(new uint[npoints]);
		for (size_t i = 0; i < npoints; ++i)
			indices[i] = (uint) i;

		self->draw_shape(
			GL_LINE_STRIP, (int) npoints, indices.get(),
			(int) vertex_size, (int) vertex_stride, points);
	}

	void
	Painter::lines (const Point* points, size_t size)
	{
		draw_lines(this, (const coord*) points, size, 2, sizeof(Point));
	}

	void
	draw_polygon (
		Painter* painter, const coord* points, size_t npoints,
		size_t vertex_size, size_t vertex_stride = 0)
	{
		assert(painter);

		Painter::Data* self = painter->self.get();

		if (!self->painting)
			invalid_state_error(__FILE__, __LINE__, "self->painting should be true.");

		if (!self->has_color()) return;

		GLenum modes[] = {GL_TRIANGLE_FAN, GL_LINE_LOOP};

		boost::scoped_array<uint> indices(new uint[npoints]);
		for (size_t i = 0; i < npoints; ++i)
			indices[i] = (uint) i;

		for (int type = COLOR_TYPE_BEGIN; type < COLOR_TYPE_END; ++type)
		{
			if (!self->use_color((ColorType) type))
				continue;

			self->draw_shape(
				modes[type], (int) npoints, indices.get(),
				(int) vertex_size, (int) vertex_stride, points);
		}
	}

	void
	Painter::polygon (const Point* points, size_t size)
	{
		draw_polygon(this, (const coord*) points, size, 2, sizeof(Point));
	}

	static void
	draw_rect (Painter* painter, coord x, coord y, coord width, coord height)
	{
		static const GLenum MODES[] = {GL_TRIANGLE_FAN, GL_LINE_LOOP};
		static const uint INDICES[] =
		{
			0, 1, 2, 3
		};

		assert(painter);

		Painter::Data* self = painter->self.get();

		if (!self->painting)
			invalid_state_error(__FILE__, __LINE__, "self->painting should be true.");

		if (!self->has_color())
			return;

		coord x2 = x + width  - 1;
		coord y2 = y + height - 1;

		for (int type = COLOR_TYPE_BEGIN; type < COLOR_TYPE_END; ++type)
		{
			if (!self->use_color((ColorType) type))
				continue;

			coord xx = x2 + 1 - type;
			coord yy = y2 + 1 - type;
			coord vertices[] =
			{
				x,  y,
				x,  yy,
				xx, yy,
				xx, y
			};

			self->draw_shape(MODES[type], 4, INDICES, 2, 0, vertices);
		}
	}

	static void
	fix_rounds (
		coord* left_top, coord* right_top, coord* left_bottom, coord* right_bottom,
		coord width, coord height)
	{
		assert(left_top && right_top && left_bottom && right_bottom);

		if (width  < 0) width  = -width;
		if (height < 0) height = -height;

		coord* rounds[] = {left_top, right_top, right_bottom, left_bottom, left_top};
		coord sizes[]   = {width, height, width, height};

		for (int i = 0; i < 4; ++i)
		{
			const coord& size = sizes[i];

			coord* a = rounds[i];
			coord* b = rounds[i + 1];
			if (*a + *b <= size)
				continue;

			if (*a > *b)
				std::swap(a, b);

			if (*a * 2 > size)
				*a = *b = size / 2;
			else
				*b = size - *a;
		}
	}

	struct RoundedCorner
	{
		coord x, y, offset_sign_x, offset_sign_y, round;
	};

	static int
	draw_round (
		Coord2* vertex, const RoundedCorner& corner,
		coord sign_x, coord sign_y, float radian_offset, uint nsegment)
	{
		if (corner.round <= 0)
		{
			vertex->reset(corner.x, corner.y);
			return 1;
		}

		coord x = corner.x + corner.round * corner.offset_sign_x * sign_x;
		coord y = corner.y + corner.round * corner.offset_sign_y * sign_y;

		for (uint seg = 0; seg <= nsegment; ++seg, ++vertex)
		{
			float pos    = (float) seg / (float) nsegment;
			float radian = radian_offset + pos * PI_HALF;
			float xx     =  cos(radian);
			float yy     = -sin(radian);
			vertex->reset(
				x + xx * corner.round * sign_x,
				y + yy * corner.round * sign_y);
		}
		return nsegment + 1;
	}

	static void
	draw_round_rect (
		Painter* painter,
		coord x, coord y, coord width, coord height,
		coord round_left_top,    coord round_right_top,
		coord round_left_bottom, coord round_right_bottom,
		uint nsegment)
	{
		static const GLenum MODES[] = {GL_TRIANGLE_FAN, GL_LINE_LOOP};

		assert(painter);

		Painter::Data* self = painter->self.get();

		if (!self->painting)
			invalid_state_error(__FILE__, __LINE__, "self->painting should be true.");

		if (!self->has_color())
			return;

		if (nsegment <= 0)
			nsegment = Painter::NSEGMENT_ROUND;

		fix_rounds(
			&round_left_top,    &round_right_top,
			&round_left_bottom, &round_right_bottom,
			width, height);

		coord xx     = x + width;
		coord yy     = y + height;
		coord sign_x = width  >= 0 ? +1 : -1;
		coord sign_y = height >= 0 ? +1 : -1;

		RoundedCorner corners[] =
		{
			{xx, y,  -1, +1, round_right_top},
			{x,  y,  +1, +1, round_left_top},
			{x,  yy, +1, -1, round_left_bottom},
			{xx, yy, -1, -1, round_right_bottom}
		};

		int nvertices = 0;
		for (int i = 0; i < 4; ++i)
			nvertices += corners[i].round > 0 ? nsegment + 1 : 1;

		boost::scoped_array<uint> indices(new uint[nvertices]);
		for (int i = 0; i < nvertices; ++i)
			indices[i] = i;

		boost::scoped_array<Coord2> vertices(new Coord2[nvertices]);
		Coord2* vertex = vertices.get();
		assert(vertex);

		for (int i = 0; i < 4; ++i)
		{
			vertex += draw_round(
				vertex, corners[i], sign_x, sign_y, PI_HALF * i, nsegment);
		}

		for (int type = COLOR_TYPE_BEGIN; type < COLOR_TYPE_END; ++type)
		{
			if (!self->use_color((ColorType) type))
				continue;

			self->draw_shape(
				MODES[type], nvertices, indices.get(), 2, 0, (coord*) vertices.get());
		}
	}

	void
	Painter::rect (
		coord x, coord y, coord width, coord height,
		coord round, uint nsegment)
	{
		rect(x, y, width, height, round, round, round, round, nsegment);
	}

	void
	Painter::rect (
		coord x, coord y, coord width, coord height,
		coord round_left_top,    coord round_right_top,
		coord round_left_bottom, coord round_right_bottom,
		uint nsegment)
	{
		if (
			round_left_top    == 0 && round_right_top    == 0 &&
			round_left_bottom == 0 && round_right_bottom == 0)
		{
			draw_rect(this, x, y, width, height);
		}
		else
		{
			draw_round_rect(
				this, x, y, width, height,
				round_left_top, round_right_top, round_left_bottom, round_right_bottom,
				nsegment);
		}
	}

	void
	Painter::rect (const Bounds& bounds, coord round, uint nsegment)
	{
		rect(
			bounds.x, bounds.y, bounds.width, bounds.height,
			round, nsegment);
	}

	void
	Painter::rect (
		const Bounds& bounds,
		coord round_left_top,    coord round_right_top,
		coord round_left_bottom, coord round_right_bottom,
		uint nsegment)
	{
		rect(
			bounds.x, bounds.y, bounds.width, bounds.height,
			round_left_top, round_right_top, round_left_bottom, round_right_bottom,
			nsegment);
	}

	static void
	draw_ellipse (
		Painter* painter,
		coord x, coord y, coord width, coord height,
		float angle_from, float angle_to, coord radius_min,
		uint nsegment)
	{
		assert(painter);

		Painter::Data* self = painter->self.get();

		if (!self->painting)
			invalid_state_error(__FILE__, __LINE__, "self->painting should be true.");

		if (nsegment <= 0)
			nsegment = Painter::NSEGMENT_ELLIPSE;

		coord radius_x     = width  / 2;
		coord radius_y     = height / 2;
		coord radius_x_min = radius_x * radius_min;
		coord radius_y_min = radius_y * radius_min;
		float from         = angle_from / 360.f;
		float to           = angle_to   / 360.f;
		bool hole          = radius_min != 0;
		int nvertices      = hole ? nsegment * 2 : nsegment;
		GLenum modes[]     =
		{
			(GLenum) (hole ? GL_TRIANGLE_STRIP : GL_TRIANGLE_FAN),
			GL_LINE_LOOP
		};

		x += radius_x;
		y += radius_y;

		boost::scoped_array<uint> indices(new uint[nvertices]);
		for (int i = 0; i < nvertices; ++i)
			indices[i] = i;

		boost::scoped_array<Coord2> vertices(new Coord2[nvertices]);
		Coord2* vertex = vertices.get();
		assert(vertex);

		for (uint seg = 0; seg < nsegment; ++seg, ++vertex)
		{
			float pos    = (float) seg / (float) nsegment;
			float radian = (from + (to - from) * pos) * PI_2;
			float xx     = cos(radian);
			float yy     = -sin(radian);

			if (hole)
				vertex->reset(x + xx * radius_x_min, y + yy * radius_y_min);
			vertex  ->reset(x + xx * radius_x,     y + yy * radius_y);
		}

		for (int type = COLOR_TYPE_BEGIN; type < COLOR_TYPE_END; ++type)
		{
			if (!self->use_color((ColorType) type))
				continue;

			self->draw_shape(
				modes[type], nvertices, indices.get(), 2, 0, (coord*) vertices.get());
		}
	}

	void
	Painter::ellipse (
		coord x, coord y, coord width, coord height,
		coord radius_min, uint nsegment)
	{
		draw_ellipse(this, x, y, width, height, 0, 360, radius_min, nsegment);
	}

	void
	Painter::ellipse (const Bounds& bounds, coord radius_min, uint nsegment)
	{
		ellipse(
			bounds.x, bounds.y, bounds.width, bounds.height, radius_min, nsegment);
	}

	void
	Painter::ellipse (
		const Point& center, coord radius, coord radius_min, uint nsegment)
	{
		ellipse(
			center.x - radius, center.y - radius, radius * 2, radius * 2,
			radius_min, nsegment);
	}

	void
	Painter::arc (
		coord x, coord y, coord width, coord height,
		float angle_from, float angle_to, coord radius_min, uint nsegment)
	{
		draw_ellipse(
			this, x, y, width, height, angle_from, angle_to, radius_min, nsegment);
	}

	void
	Painter::arc (
		const Bounds& bounds,
		float angle_from, float angle_to, coord radius_min, uint nsegment)
	{
		arc(
			bounds.x, bounds.y, bounds.width, bounds.height,
			angle_from, angle_to, radius_min, nsegment);
	}

	void
	Painter::arc (
		const Point& center, coord radius,
		float angle_from, float angle_to, coord radius_min, uint nsegment)
	{
		arc(
			center.x, center.y, radius * 2, radius * 2,
			angle_from, angle_to, radius_min, nsegment);
	}

	static void
	draw_texture (
		Painter* painter, const Texture& tex,
		float s_min, float t_min, float s_max, float t_max,
		coord x, coord y, coord width, coord height,
		bool nostroke = false)
	{
		static const GLenum MODES[] = {GL_TRIANGLE_FAN, GL_LINE_LOOP};
		static const uint INDICES[] =
		{
			0, 1, 2, 3
		};

		assert(painter && tex);

		Painter::Data* self = painter->self.get();

		if (!self->painting)
			invalid_state_error(__FILE__, __LINE__, "self->painting should be true.");

		coord x2 = x + width  - 1;
		coord y2 = y + height - 1;
		coord vertices[] =
		{
			x,  y,
			x,  y2,
			x2, y2,
			x2, y
		};

		for (int type = COLOR_TYPE_BEGIN; type < COLOR_TYPE_END; ++type)
		{
			if (
				(nostroke && type == STROKE) ||
				!self->use_color((ColorType) type))
			{
				continue;
			}

			if (type == FILL)
			{
				coord tex_coords[] =
				{
					s_min, t_min,
					s_min, t_max,
					s_max, t_max,
					s_max, t_min
				};
				self->draw_shape(MODES[type], 4, INDICES, 2, 0, vertices, tex_coords, &tex);
			}
			else
				self->draw_shape(MODES[type], 4, INDICES, 2, 0, vertices);
		}
	}

	void
	Painter::image (const Image& image_, coord x, coord y)
	{
		if (!image_)
			argument_error(__FILE__, __LINE__);

		const Texture& tex = image_.texture();
		if (!tex)
			argument_error(__FILE__, __LINE__);

		draw_texture(
			this, tex,
			0, 0, tex.s_max(), tex.t_max(),
			x, y, tex.width(), tex.height());
	}

	void
	Painter::image (const Image& image_, const Point& position)
	{
		image(image_, position.x, position.y);
	}

	void
	Painter::image (
		const Image& image_, coord x, coord y, coord width, coord height)
	{
		if (!image_)
			argument_error(__FILE__, __LINE__);

		const Texture& tex = image_.texture();
		if (!tex)
			argument_error(__FILE__, __LINE__);

		draw_texture(
			this, tex,
			0, 0, tex.s_max(), tex.t_max(),
			x, y, width,       height);
	}

	void
	Painter::image (
		const Image& image_, const Bounds& bounds)
	{
		image(image_, bounds.x, bounds.y, bounds.width, bounds.height);
	}

	void
	Painter::image (
		const Image& image_,
		coord  src_x, coord  src_y, coord src_width, coord src_height,
		coord dest_x, coord dest_y)
	{
		if (!image_)
			argument_error(__FILE__, __LINE__);

		const Texture& tex = image_.texture();
		if (!tex)
			argument_error(__FILE__, __LINE__);

		coord dest_width = tex.width(), dest_height = tex.height();
		float s = tex.s_max() / dest_width, t = tex.t_max() / dest_height;
		draw_texture(
			this, tex,
			 src_x * s,  src_y * t,  src_width * s,  src_height * t,
			dest_x,     dest_y,     dest_width,     dest_height);
	}

	void
	Painter::image (
		const Image& image_, const Bounds& src_bounds, const Point& dest_position)
	{
		image(
			image_,
			src_bounds.x, src_bounds.y, src_bounds.width, src_bounds.height,
			dest_position.x, dest_position.y);
	}

	void
	Painter::image (
		const Image& image_,
		coord  src_x, coord  src_y, coord  src_width, coord  src_height,
		coord dest_x, coord dest_y, coord dest_width, coord dest_height)
	{
		if (!image_)
			argument_error(__FILE__, __LINE__);

		const Texture& tex = image_.texture();
		if (!tex)
			argument_error(__FILE__, __LINE__);

		float s = tex.s_max() / tex.width();
		float t = tex.t_max() / tex.height();
		draw_texture(
			this, tex,
			 src_x * s,  src_y * t,  src_width * s,  src_height * t,
			dest_x,     dest_y,     dest_width,     dest_height);
	}

	void
	Painter::image (
		const Image& image_, const Bounds& src_bounds, const Bounds& dest_bounds)
	{
		image(
			image_,
			 src_bounds.x,  src_bounds.y,  src_bounds.width,  src_bounds.height,
			dest_bounds.x, dest_bounds.y, dest_bounds.width, dest_bounds.height);
	}

	static void
	draw_text (
		Painter* painter, const char* str, coord str_width, coord str_height,
		coord x, coord y, coord width, coord height,
		const Font& font)
	{
		assert(painter && str && *str != '\0' && font);

		if (!painter->self->painting)
			invalid_state_error(__FILE__, __LINE__, "self->painting should be true.");

		Painter::Data* self = painter->self.get();
		int tex_w = ceil(str_width);
		int tex_h = ceil(str_height);

		if (
			self->text_image.width()  < tex_w ||
			self->text_image.height() < tex_h)
		{
			self->text_image = Image(
				std::max(self->text_image.width(),  tex_w),
				std::max(self->text_image.height(), tex_h),
				self->text_image.color_space(),
				self->text_image.alpha_only());
		}

		if (!self->text_image)
			invalid_state_error(__FILE__, __LINE__);

		draw_string(&self->text_image.bitmap(), str, 0, 0, font);

		const Texture& tex = self->text_image.texture();
		if (!tex)
			rays_error(__FILE__, __LINE__, "text_image's texture is invalid.");

		#if 0//def DEBUG
			save_image(self->text_image, "/Users/snori/font.png");

			painter->push_attrs();
			{
				coord asc, desc, lead;
				font.get_height(&asc, &desc, &lead);
				//printf("%f %f %f %f \n", str_height, asc, desc, lead);

				painter->set_stroke(0.5, 0.5, 1);
				painter->no_fill();
				painter->rect(x - 1, y - 1, str_width + 2, str_height + 2);

				coord yy = y;
				painter->set_stroke(1, 0.5, 0.5, 0.4);
				painter->rect(x, yy, str_width, asc);//str_height);

				yy += asc;
				painter->set_stroke(1, 1, 0.5, 0.4);
				painter->rect(x, yy, str_width, desc);

				yy += desc;
				painter->set_stroke(1, 0.5, 1, 0.4);
				painter->rect(x, yy, str_width, lead);
			}
			painter->pop_attr();
		#endif

		draw_texture(
			painter, tex,
			0, 0, tex.s(str_width - 1), tex.t(str_height - 1),
			x, y, width,                height,
			true);
	}

	void
	Painter::text (const char* str, coord x, coord y, const Font* font)
	{
		if (!str)
			argument_error(__FILE__, __LINE__);

		if (*str == '\0') return;

		if (!font) font = &self->attrs.font;
		if (!*font)
			argument_error(__FILE__, __LINE__);

		coord w = font->get_width(str), h = font->get_height();
		w = ceil(w);
		h = ceil(h);
		draw_text(this, str, w, h, x, y, w, h, *font);
	}

	void
	Painter::text (const char* str, const Point& position, const Font* font)
	{
		text(str, position.x, position.y, font);
	}

	void
	Painter::text (
		const char* str, coord x, coord y, coord width, coord height,
		const Font* font)
	{
		if (!str)
			argument_error(__FILE__, __LINE__);

		if (*str == '\0') return;

		if (!font) font = &self->attrs.font;
		if (!*font)
			argument_error(__FILE__, __LINE__);

		coord w = font->get_width(str), h = font->get_height();
		w = ceil(w);
		h = ceil(h);
		draw_text(this, str, w, h, x, y, width, height, *font);
	}

	void
	Painter::text (const char* str, const Bounds& bounds, const Font* font)
	{
		text(str, bounds.x, bounds.y, bounds.width, bounds.height, font);
	}


	void
	Painter::set_background (float red, float green, float blue, float alpha, bool clear)
	{
		set_background(Color(red, green, blue, alpha), clear);
	}

	void
	Painter::set_background (const Color& color, bool clear)
	{
		self->attrs.background = color;

		if (self->painting && clear) this->clear();
	}

	void
	Painter::no_background (bool clear)
	{
		Color c = background();
		c.alpha = 0;
		set_background(c, clear);
	}

	const Color&
	Painter::background () const
	{
		return self->attrs.background;
	}

	void
	Painter::set_fill (float red, float green, float blue, float alpha)
	{
		set_fill(Color(red, green, blue, alpha));
	}

	void
	Painter::set_fill (const Color& color)
	{
		self->attrs.colors[FILL] = color;
	}

	void
	Painter::no_fill ()
	{
		self->attrs.colors[FILL].alpha = 0;
	}

	const Color&
	Painter::fill () const
	{
		return self->attrs.colors[FILL];
	}

	void
	Painter::set_stroke (float red, float green, float blue, float alpha)
	{
		set_stroke(Color(red, green, blue, alpha));
	}

	void
	Painter::set_stroke (const Color& color)
	{
		self->attrs.colors[STROKE] = color;
	}

	void
	Painter::no_stroke ()
	{
		self->attrs.colors[STROKE].alpha = 0;
	}

	const Color&
	Painter::stroke () const
	{
		return self->attrs.colors[STROKE];
	}

	void
	Painter::set_clip (coord x, coord y, coord width, coord height)
	{
		set_clip(Bounds(x, y, width, height));
	}

	void
	Painter::set_clip (const Bounds& bounds)
	{
		self->attrs.clip = bounds;
		self->update_clip();
	}

	void
	Painter::no_clip ()
	{
		set_clip(0, 0, -1, -1);
	}

	const Bounds&
	Painter::clip () const
	{
		return self->attrs.clip;
	}

	void
	Painter::set_font (const char* name, coord size)
	{
		set_font(Font(name, size));
	}

	void
	Painter::set_font (const Font& font)
	{
		self->attrs.font = font;
	}

	const Font&
	Painter::font () const
	{
		return self->attrs.font;
	}

	void
	Painter::push_attrs ()
	{
		self->attrs_stack.push_back(self->attrs);
	}

	void
	Painter::pop_attrs ()
	{
		if (self->attrs_stack.empty())
			rays_error(__FILE__, __LINE__, "attrs_stack is empty.");

		self->attrs = self->attrs_stack.back();
		self->attrs_stack.pop_back();
		self->update_clip();
	}


	void
	Painter::attach (const Shader& shader)
	{
		self->program.attach(shader);
	}

	void
	Painter::detach (const Shader& shader)
	{
		self->program.detach(shader);
	}

	void
	Painter::set_uniform (const char* name, int arg1)
	{
		self->program.set_uniform(name, arg1);
	}

	void
	Painter::set_uniform (const char* name, int arg1, int arg2)
	{
		self->program.set_uniform(name, arg1, arg2);
	}

	void
	Painter::set_uniform (const char* name, int arg1, int arg2, int arg3)
	{
		self->program.set_uniform(name, arg1, arg2, arg3);
	}

	void
	Painter::set_uniform (const char* name, int arg1, int arg2, int arg3, int arg4)
	{
		self->program.set_uniform(name, arg1, arg2, arg3, arg4);
	}

	void
	Painter::set_uniform (const char* name, const int* args, size_t size)
	{
		self->program.set_uniform(name, args, size);
	}

	void
	Painter::set_uniform (const char* name, float arg1)
	{
		self->program.set_uniform(name, arg1);
	}

	void
	Painter::set_uniform (const char* name, float arg1, float arg2)
	{
		self->program.set_uniform(name, arg1, arg2);
	}

	void
	Painter::set_uniform (const char* name, float arg1, float arg2, float arg3)
	{
		self->program.set_uniform(name, arg1, arg2, arg3);
	}

	void
	Painter::set_uniform (const char* name, float arg1, float arg2, float arg3, float arg4)
	{
		self->program.set_uniform(name, arg1, arg2, arg3, arg4);
	}

	void
	Painter::set_uniform (const char* name, const float* args, size_t size)
	{
		self->program.set_uniform(name, args, size);
	}

	void
	Painter::push_shaders ()
	{
		self->program.push();
	}

	void
	Painter::pop_shaders ()
	{
		self->program.pop();
	}


	void
	Painter::translate (coord x, coord y, coord z)
	{
		glTranslatef(x, y, z);
	}

	void
	Painter::translate (const Point& value)
	{
		translate(value.x, value.y, value.z);
	}

	void
	Painter::scale (coord x, coord y, coord z)
	{
		glScalef(x, y, z);
	}

	void
	Painter::scale (const Point& value)
	{
		scale(value.x, value.y, value.z);
	}

	void
	Painter::rotate (float angle, coord x, coord y, coord z)
	{
		glRotatef(angle, x, y, z);
	}

	void
	Painter::rotate (float angle, const Point& axis)
	{
		rotate(angle, axis.x, axis.y, axis.z);
	}

	void
	Painter::set_matrix (float value)
	{
		if (value == 1)
		{
			glLoadIdentity();
			return;
		}

		set_matrix(
			value, 0, 0, 0,
			0, value, 0, 0,
			0, 0, value, 0,
			0, 0, 0, value);
	}

	void
	Painter::set_matrix (
		float a1, float a2, float a3, float a4,
		float b1, float b2, float b3, float b4,
		float c1, float c2, float c3, float c4,
		float d1, float d2, float d3, float d4)
	{
		float array[] =
		{
			a1, a2, a3, a4,
			b1, b2, b3, b4,
			c1, c2, c3, c4,
			d1, d2, d3, d4
		};
		set_matrix(array);
	}

	void
	Painter::set_matrix (const float* elements)
	{
		if (!elements)
			argument_error(__FILE__, __LINE__);

		glLoadMatrixf(elements);
	}

	void
	Painter::set_matrix (const Matrix& matrix)
	{
		set_matrix(matrix.array);
	}

	const Matrix&
	Painter::matrix () const
	{
		glGetFloatv(GL_MODELVIEW_MATRIX, self->matrix_tmp.array);
		check_error(__FILE__, __LINE__);
		return self->matrix_tmp;
	}

	void
	Painter::push_matrix ()
	{
		glPushMatrix();
		check_error(__FILE__, __LINE__);
	}

	void
	Painter::pop_matrix ()
	{
		glPopMatrix();
		check_error(__FILE__, __LINE__);
	}


	Painter::operator bool () const
	{
		return self->viewport;
	}

	bool
	Painter::operator ! () const
	{
		return !operator bool();
	}


}// Rays
