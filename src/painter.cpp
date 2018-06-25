#include "painter.h"


#include <math.h>
#include <assert.h>
#include <memory>
#include <vector>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include "rays/exception.h"
#include "rays/bounds.h"
#include "rays/color.h"
#include "rays/debug.h"
#include "opengl.h"
#include "point.h"
#include "matrix.h"
#include "polygon.h"
#include "bitmap.h"
#include "texture.h"
#include "image.h"
#include "font.h"
#include "frame_buffer.h"
#include "shader.h"
#include "shader_program.h"
#include "shader_source.h"


namespace Rays
{


	enum ColorType
	{

		FILL = 0,
		STROKE,

		COLOR_TYPE_END,
		COLOR_TYPE_BEGIN = 0

	};// ColorType


	struct State
	{

		Bounds clip;

		Color background, colors[2];

		Font font;

		Shader shader;

		void init ()
		{
			clip           .reset(-1);
			background     .reset(0, 0);
			colors[FILL]   .reset(1, 1);
			colors[STROKE] .reset(1, 0);
			font           = default_font();
			shader         = Shader();
		}

		bool has_color ()
		{
			return colors[FILL] || colors[STROKE];
		}

	};// State


	struct Vertex
	{

		typedef Coord4 Position;
		typedef Coord4 TexCoord;

		union
		{
			struct {coord x, y, z;};
			Position position;
		};

		union
		{
			struct {float s, t;};
			TexCoord texcoord;
		};

		void reset (coord x, coord y, coord s, coord t)
		{
			position.reset(x, y);
			texcoord.reset(s, t);
		}

	};// Vertex


	struct TextureInfo
	{

		const Texture& texture;

		Coord2 texcoord_min, texcoord_max;

		TextureInfo (
			const Texture& texture,
			coord x_min, coord y_min,
			coord x_max, coord y_max)
		:	texture(texture)
		{
			texcoord_min.reset(x_min, y_min);
			texcoord_max.reset(x_max, y_max);
		}

		operator bool () const
		{
			return
				texture &&
				texcoord_min.x < texcoord_max.x &&
				texcoord_min.x < texcoord_max.y;
		}

		bool operator ! () const
		{
			return !operator bool();
		}

	};// TextureInfo


	struct OpenGLState
	{

		GLint viewport[4];

		GLclampf color_clear[4];

		GLboolean scissor_test;
		GLint scissor_box[4];

		GLboolean blend;
		GLint blend_src_factor, blend_dst_factor;

		GLint framebuffer_binding;

		void push ()
		{
			glGetIntegerv(GL_VIEWPORT, viewport);

			glGetFloatv(GL_COLOR_CLEAR_VALUE, color_clear);

			glGetBooleanv(GL_SCISSOR_TEST, &scissor_test);
			glGetIntegerv(GL_SCISSOR_BOX, scissor_box);

			glGetBooleanv(GL_BLEND, &blend);
			glGetIntegerv(GL_BLEND_SRC_ALPHA, &blend_src_factor);
			glGetIntegerv(GL_BLEND_DST_ALPHA, &blend_dst_factor);

			glGetIntegerv(GL_FRAMEBUFFER_BINDING, &framebuffer_binding);
		}

		void pop ()
		{
			glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

			glClearColor(
				color_clear[0], color_clear[1], color_clear[2], color_clear[3]);

			enable(GL_SCISSOR_TEST, scissor_test);
			glScissor(scissor_box[0], scissor_box[1], scissor_box[2], scissor_box[3]);

			enable(GL_BLEND, blend);
			glBlendFunc(blend_src_factor, blend_dst_factor);

			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_binding);
		}

		private:

			void enable(GLenum type, GLboolean value)
			{
				if (value)
					glEnable(type);
				else
					glDisable(type);
			}

	};// OpenGLState


	static constexpr float PI      = M_PI;
	static constexpr float PI_2    = PI * 2;
	static constexpr float PI_HALF = PI / 2;


	#define  ATTRIB_POSITION        "a_Position"
	#define VARYING_POSITION        "v_Position"
	#define UNIFORM_POSITION_MATRIX "u_PositionMatrix"
	#define  ATTRIB_TEXCOORD        "a_TexCoord"
	#define VARYING_TEXCOORD        "v_TexCoord"
	#define UNIFORM_TEXCOORD_MIN    "u_TexCoordMin"
	#define UNIFORM_TEXCOORD_MAX    "u_TexCoordMax"
	#define UNIFORM_TEXCOORD_MATRIX "u_TexCoordMatrix"
	#define  ATTRIB_COLOR           "a_Color"
	#define VARYING_COLOR           "v_Color"
	//#define UNIFORM_COLOR_MATRIX    "u_ColorMatrix"
	#define UNIFORM_TEXTURE         "u_Texture"
	#define UNIFORM_TEXTURE_SIZE    "u_TextureSize"


	template <typename COORD>
	static GLenum get_gl_type ();

	template <>
	GLenum
	get_gl_type<float> ()
	{
		return GL_FLOAT;
	}

	const ShaderSource&
	Painter_get_vertex_shader_source ()
	{
		static const ShaderSource SOURCE(
			GL_VERTEX_SHADER,
			"attribute vec4 "  ATTRIB_POSITION ";"
			"varying   vec4 " VARYING_POSITION ";"
			"uniform   mat4 " UNIFORM_POSITION_MATRIX ";"
			"attribute vec4 "  ATTRIB_TEXCOORD ";"
			"varying   vec4 " VARYING_TEXCOORD ";"
			"uniform   mat4 " UNIFORM_TEXCOORD_MATRIX ";"
			"attribute vec4 "  ATTRIB_COLOR ";"
			"varying   vec4 " VARYING_COLOR ";"
			"void main ()"
			"{"
			   VARYING_POSITION " = " ATTRIB_POSITION ";"
			   VARYING_COLOR    " = " ATTRIB_COLOR ";"
			   VARYING_TEXCOORD " = " UNIFORM_TEXCOORD_MATRIX " * " ATTRIB_TEXCOORD ";"
			"  gl_Position        = " UNIFORM_POSITION_MATRIX " * " ATTRIB_POSITION ";"
			"}");
		return SOURCE;
	}

	const ShaderSource&
	Painter_get_fragment_shader_shared_source ()
	{
		static const ShaderSource SHARED_SOURCE(
			GL_FRAGMENT_SHADER,
			"uniform sampler2D " UNIFORM_TEXTURE ";"
			"uniform vec2 "      UNIFORM_TEXTURE_SIZE ";"
			"uniform vec2 "      UNIFORM_TEXCOORD_MIN ";"
			"uniform vec2 "      UNIFORM_TEXCOORD_MAX ";"
			"vec2 normalizeTexCoord(vec2 texcoord)"
			"{"
			"  vec2 min = " UNIFORM_TEXCOORD_MIN ";"
			"  vec2 len = " UNIFORM_TEXCOORD_MAX " - min;"
			"  return (mod(texcoord - min, len) + min) / " UNIFORM_TEXTURE_SIZE ";"
			"}"
			"vec4 sampleTexture(vec2 texcoord)"
			"{"
			"  return texture2D(" UNIFORM_TEXTURE ", normalizeTexCoord(texcoord));"
			"}");
		return SHARED_SOURCE;
	}


	static const Shader&
	get_default_shader_for_shape ()
	{
		static const Shader SHADER(
			"varying vec4 " VARYING_COLOR ";"
			"void main ()"
			"{"
			"  gl_FragColor = v_Color;"
			"}");
		return SHADER;
	}

	static const Shader&
	get_default_shader_for_color_texture ()
	{
		static const Shader SHADER(
			"varying vec4 " VARYING_COLOR ";"
			"varying vec4 " VARYING_TEXCOORD ";"
			"vec4 sampleTexture(vec2);"
			"void main ()"
			"{"
			"  vec4 color   = sampleTexture(" VARYING_TEXCOORD ".xy);"
			"  gl_FragColor = v_Color * color;"
			"}");
		return SHADER;
	}

	static const Shader&
	get_default_shader_for_alpha_texture ()
	{
		static const Shader SHADER(
			"varying vec4 " VARYING_COLOR ";"
			"varying vec4 " VARYING_TEXCOORD ";"
			"vec4 sampleTexture(vec2);"
			"void main ()"
			"{"
			"  vec4 color   = sampleTexture(" VARYING_TEXCOORD ".xy);"
			"  gl_FragColor = vec4(v_Color.rgb, color.a);"
			"}");
		return SHADER;
	}


	struct Painter::Data
	{

		bool painting = false;

		float pixel_density = 1;

		Bounds viewport;

		State              state;

		std::vector<State> state_stack;

		Matrix              position_matrix;

		std::vector<Matrix> position_matrix_stack;

		FrameBuffer frame_buffer;

		Image text_image;

		OpenGLState opengl_state;

		Data ()
		{
			state.init();
		}

		void set_pixel_density (float density)
		{
			if (density <= 0)
				argument_error(__FILE__, __LINE__, "invalid pixel_density.");

			this->pixel_density = density;
			text_image = Image();
		}

		void update_clip ()
		{
			const Bounds& clip = state.clip;
			if (clip)
			{
				coord y = frame_buffer ? clip.y : viewport.h - (clip.y + clip.h);
				glEnable(GL_SCISSOR_TEST);
				glScissor(
					pixel_density * clip.x,
					pixel_density * y,
					pixel_density * clip.width,
					pixel_density * clip.height);
			}
			else
			{
				glDisable(GL_SCISSOR_TEST);
			}

			check_error(__FILE__, __LINE__);
		}

		bool get_color (Color* color, ColorType type)
		{
			const Color& c = state.colors[type];
			if (!c) return false;

			*color = c;
			return true;
		}

		void draw_shape (
			GLenum mode,
			const Vertex* vertices, size_t nvertices,
			const uint*   indices,  size_t nindices,
			const Color& color,
			const Shader& default_shader,
			const TextureInfo* texinfo = NULL)
		{
			if (nvertices <= 0 || nindices <= 0 || !vertices || !indices)
				argument_error(__FILE__, __LINE__);

			if (!painting)
				invalid_state_error(__FILE__, __LINE__, "'painting' should be true.");

			const ShaderProgram* program = Shader_get_program(state.shader);
			if (!program || !*program)
			{
				program = Shader_get_program(default_shader);
				if (!program || !*program) return;
			}

			ShaderProgram_activate(*program);
			apply_builtin_uniforms(*program, texinfo);

			GLint a_position = glGetAttribLocation(program->id(), ATTRIB_POSITION);
			GLint a_color    = glGetAttribLocation(program->id(), ATTRIB_COLOR);
			GLint a_texcoord = glGetAttribLocation(program->id(), ATTRIB_TEXCOORD);
			if (a_position < 0 || a_color < 0 || a_texcoord < 0)
				opengl_error(__FILE__, __LINE__);

			setup_vertices(
				vertices, nvertices, color, a_position, a_color, a_texcoord);
			//activate_texture(texture);

			glDrawElements(mode, (GLsizei) nindices, GL_UNSIGNED_INT, indices);
			check_error(__FILE__, __LINE__);

			//deactivate_texture(texture);
			cleanup_vertices(a_position, a_texcoord);

			ShaderProgram_deactivate();
		}

		private:

			void apply_builtin_uniforms (
				const ShaderProgram& program, const TextureInfo* texinfo)
			{
				GLint pos_matrix_loc =
					glGetUniformLocation(program.id(), UNIFORM_POSITION_MATRIX);
				if (pos_matrix_loc >= 0)
				{
					glUniformMatrix4fv(
						pos_matrix_loc, 1, GL_FALSE, position_matrix.array);
					check_error(__FILE__, __LINE__);
				}

				GLint texcoord_matrix_loc =
					glGetUniformLocation(program.id(), UNIFORM_TEXCOORD_MATRIX);
				if (texcoord_matrix_loc >= 0)
				{
					static const Matrix TEXCOORD_MATRIX(1);
					glUniformMatrix4fv(
						texcoord_matrix_loc, 1, GL_FALSE, TEXCOORD_MATRIX.array);
					check_error(__FILE__, __LINE__);
				}

				apply_texture_uniforms(program, texinfo);
			}

			void apply_texture_uniforms (
				const ShaderProgram& program, const TextureInfo* texinfo)
			{
				if (!texinfo || !*texinfo) return;

				const Texture& texture = texinfo->texture;

				GLint texture_loc =
					glGetUniformLocation(program.id(), UNIFORM_TEXTURE);
				if (texture_loc >= 0)
				{
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, texture.id());

					glUniform1i(texture_loc, 0);
					check_error(__FILE__, __LINE__);
				}

				GLint size_loc =
					glGetUniformLocation(program.id(), UNIFORM_TEXTURE_SIZE);
				if (size_loc >= 0)
				{
					glUniform2f(
						size_loc, texture.reserved_width(), texture.reserved_height());
					check_error(__FILE__, __LINE__);
				}

				GLint min_loc =
					glGetUniformLocation(program.id(), UNIFORM_TEXCOORD_MIN);
				if (min_loc >= 0)
				{
					glUniform2fv(min_loc, 1, texinfo->texcoord_min.array);
					check_error(__FILE__, __LINE__);
				}

				GLint max_loc =
					glGetUniformLocation(program.id(), UNIFORM_TEXCOORD_MAX);
				if (max_loc >= 0)
				{
					glUniform2fv(max_loc, 1, texinfo->texcoord_max.array);
					check_error(__FILE__, __LINE__);
				}
			}

			void setup_vertices (
				const Vertex* vertices, size_t nvertices, const Color& color,
				GLuint a_position, GLuint a_color, GLuint a_texcoord)
			{
				assert(nvertices >= 0 && vertices);

				glVertexAttrib4fv(a_color, color.array);

				glEnableVertexAttribArray(a_position);
				glVertexAttribPointer(
					a_position, Vertex::Position::SIZE, get_gl_type<coord>(),
					GL_FALSE, sizeof(Vertex), &vertices->position);
				check_error(__FILE__, __LINE__);

				glEnableVertexAttribArray(a_texcoord);
				glVertexAttribPointer(
					a_texcoord, Vertex::TexCoord::SIZE, GL_FLOAT,
					GL_FALSE, sizeof(Vertex), &vertices->texcoord);
				check_error(__FILE__, __LINE__);
			}

			void cleanup_vertices (GLint a_position, GLint a_texcoord)
			{
				glDisableVertexAttribArray(a_position);
				glDisableVertexAttribArray(a_texcoord);
				check_error(__FILE__, __LINE__);
			}

			void activate_texture (const Texture* texture)
			{
				if (!texture)
				{
					glDisable(GL_TEXTURE_2D);
					return;
				}

				if (!*texture)
					argument_error(__FILE__, __LINE__, "invalid texture.");

				GLuint id = texture->id();
				if (id != get_current_texture_id())
					glBindTexture(GL_TEXTURE_2D, id);

				glEnable(GL_TEXTURE_2D);
			}

			GLuint get_current_texture_id ()
			{
				GLint id = 0;
				glGetIntegerv(GL_TEXTURE_BINDING_2D, &id);
				return (GLuint) id;
			}

			void deactivate_texture (const Texture* texture)
			{
				if (!texture) return;

				glDisable(GL_TEXTURE_2D);
			}

	};// Painter::Data


	Painter::Painter ()
	{
	}

	Painter::~Painter ()
	{
	}

	void
	Painter::canvas (
		coord x, coord y, coord width, coord height, float pixel_density)
	{
		canvas(Bounds(x, y, -100, width, height, 200), pixel_density);
	}

	void
	Painter::canvas (
		coord x, coord y, coord z, coord width, coord height, coord depth,
		float pixel_density)
	{
		canvas(Bounds(x, y, z, width, height, depth), pixel_density);
	}

	void
	Painter::canvas (const Bounds& viewport, float pixel_density)
	{
		if (!viewport)
			argument_error(__FILE__, __LINE__);

		if (self->painting)
			invalid_state_error(__FILE__, __LINE__, "painting flag should be false.");

		self->viewport = viewport;
		self->set_pixel_density(pixel_density);
	}

	void
	Painter::bind (const Image& image)
	{
		if (!image)
			argument_error(__FILE__, __LINE__, "invalid image.");

		if (self->painting)
			invalid_state_error(__FILE__, __LINE__, "painting flag should be false.");

		FrameBuffer fb(Image_get_texture(image));
		if (!fb)
			rays_error(__FILE__, __LINE__, "invalid frame buffer.");

		unbind();

		self->frame_buffer = fb;
		canvas(0, 0, image.width(), image.height(), image.pixel_density());
	}

	void
	Painter::unbind ()
	{
		if (self->painting)
			invalid_state_error(__FILE__, __LINE__, "painting flag should be true.");

		self->frame_buffer = FrameBuffer();
	}

	const Bounds&
	Painter::bounds () const
	{
		return self->viewport;
	}

	float
	Painter::pixel_density () const
	{
		return self->pixel_density;
	}

	void
	Painter::begin ()
	{
		if (self->painting)
			invalid_state_error(__FILE__, __LINE__, "painting flag should be false.");

		self->opengl_state.push();

		FrameBuffer& fb = self->frame_buffer;
		if (fb) FrameBuffer_bind(fb.id());

		const Bounds& vp = self->viewport;
		float density    = self->pixel_density;
		glViewport(
			(int) (vp.x      * density), (int) (vp.y      * density),
			(int) (vp.width  * density), (int) (vp.height * density));

		coord x1 = vp.x, x2 = vp.x + vp.width;
		coord y1 = vp.y, y2 = vp.y + vp.height;
		coord z1 = vp.z, z2 = vp.z + vp.depth;
		if (z1 == 0 && z2 == 0) {z1 = -100; z2 = 200;}
		if (!fb) std::swap(y1, y2);

		self->state.init();

		self->position_matrix.reset(1);
		self->position_matrix *= to_rays(glm::ortho(x1, x2, y1, y2));
		//self->position_matrix.translate(0.375f, 0.375f);

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
			invalid_state_error(__FILE__, __LINE__, "painting flag should be true.");

		if (!self->state_stack.empty())
			invalid_state_error(__FILE__, __LINE__, "state stack is not empty.");

		if (!self->position_matrix_stack.empty())
			invalid_state_error(__FILE__, __LINE__, "position matrix stack is not empty.");

		self->painting = false;
		self->opengl_state.pop();

		glFinish();

		if (self->frame_buffer)
		{
			FrameBuffer_unbind();

			Texture& tex = self->frame_buffer.texture();
			if (tex) tex.set_modified();
		}
	}

	void
	Painter::clear ()
	{
		if (!self->painting)
			invalid_state_error(__FILE__, __LINE__, "painting flag should be true.");

		const Color& c = self->state.background;
		glClearColor(c.red, c.green, c.blue, c.alpha);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		check_error(__FILE__, __LINE__);
	}

	void
	draw_line (Painter* painter, const Point* points, size_t npoints, bool loop)
	{
		assert(painter);

		Painter::Data* self = painter->self.get();

		if (!self->painting)
			invalid_state_error(__FILE__, __LINE__, "painting flag should be true.");

		if (!self->state.has_color())
			return;

		GLenum modes[] = {
			GL_TRIANGLE_FAN,
			(GLenum) (loop ? GL_LINE_LOOP : GL_LINE_STRIP)
		};

		std::unique_ptr<Vertex[]> vertices(new Vertex[npoints]);
		std::unique_ptr<uint[]>   indices(new uint[npoints]);
		for (size_t i = 0; i < npoints; ++i)
		{
			vertices[i].position = points[i];
			vertices[i].texcoord = points[i];
			indices[i]           = (uint) i;
		}

		Color color;
		for (int type = COLOR_TYPE_BEGIN; type < COLOR_TYPE_END; ++type)
		{
			if (!self->get_color(&color, (ColorType) type))
				continue;

			self->draw_shape(
				modes[type], vertices.get(), npoints, indices.get(), npoints, color,
				get_default_shader_for_shape());
		}
	}

	void
	Painter::line (coord x1, coord y1, coord x2, coord y2)
	{
		line(Point(x1, y1, 0), Point(x2, y2, 0));
	}

	void
	Painter::line (const Point& p1, const Point& p2)
	{
		const Point points[] = {p1, p2};
		draw_line(this, points, 2, false);
	}

	void
	Painter::line (const Point* points, size_t size, bool loop)
	{
		draw_line(this, points, size, loop);
	}

	static void
	draw_rect (Painter* painter, coord x, coord y, coord width, coord height)
	{
		static const GLenum MODES[] = {GL_TRIANGLE_FAN, GL_LINE_LOOP};
		static const uint INDICES[] = {0, 1, 2, 3};

		assert(painter);

		Painter::Data* self = painter->self.get();

		if (!self->painting)
			invalid_state_error(__FILE__, __LINE__, "painting flag should be true.");

		if (!self->state.has_color())
			return;

		Color color;
		Vertex vertices[4];
		for (int type = COLOR_TYPE_BEGIN; type < COLOR_TYPE_END; ++type)
		{
			if (!self->get_color(&color, (ColorType) type))
				continue;

			vertices[0].reset(x,         y,          0,     0);
			vertices[1].reset(x,         y + height, 0,     height);
			vertices[2].reset(x + width, y + height, width, height);
			vertices[3].reset(x + width, y,          width, 0);

			self->draw_shape(
				MODES[type], vertices, 4, INDICES, 4, color,
				get_default_shader_for_shape());
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

		for (size_t i = 0; i < 4; ++i)
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
	setup_round (
		Vertex* vertex, coord x, coord y, const RoundedCorner& corner,
		coord sign_x, coord sign_y, float radian_offset, uint nsegment)
	{
		if (corner.round <= 0)
		{
			vertex->reset(x + corner.x, y + corner.y, corner.x, corner.y);
			return 1;
		}

		coord offset_x = corner.x + corner.round * corner.offset_sign_x * sign_x;
		coord offset_y = corner.y + corner.round * corner.offset_sign_y * sign_y;

		for (uint seg = 0; seg <= nsegment; ++seg, ++vertex)
		{
			float pos    = (float) seg / (float) nsegment;
			float radian = radian_offset + pos * PI_HALF;
			coord xx     = offset_x + cos(radian) * corner.round * sign_x;
			coord yy     = offset_y - sin(radian) * corner.round * sign_y;
			vertex->reset(x + xx, y + yy, xx, yy);
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
			invalid_state_error(__FILE__, __LINE__, "painting flag should be true.");

		if (!self->state.has_color())
			return;

		if (nsegment <= 0)
			nsegment = NSEGMENT_ROUND;

		fix_rounds(
			&round_left_top,    &round_right_top,
			&round_left_bottom, &round_right_bottom,
			width, height);

		coord sign_x = width  >= 0 ? +1 : -1;
		coord sign_y = height >= 0 ? +1 : -1;
		RoundedCorner corners[] =
		{
			{width,      0, -1, +1, round_right_top},
			{    0,      0, +1, +1, round_left_top},
			{    0, height, +1, -1, round_left_bottom},
			{width, height, -1, -1, round_right_bottom}
		};

		size_t nvertices = 0;
		for (size_t i = 0; i < 4; ++i)
			nvertices += corners[i].round > 0 ? nsegment + 1 : 1;

		std::unique_ptr<uint[]> indices(new uint[nvertices]);
		for (size_t i = 0; i < nvertices; ++i)
			indices[i] = (int) i;

		std::unique_ptr<Vertex[]> vertices(new Vertex[nvertices]);
		Vertex* vertex = vertices.get();

		for (size_t i = 0; i < 4; ++i)
		{
			vertex += setup_round(
				vertex, x, y, corners[i], sign_x, sign_y, PI_HALF * i, nsegment);
		}

		Color color;
		for (int type = COLOR_TYPE_BEGIN; type < COLOR_TYPE_END; ++type)
		{
			if (!self->get_color(&color, (ColorType) type))
				continue;

			self->draw_shape(
				MODES[type], vertices.get(), nvertices, indices.get(), nvertices, color,
				get_default_shader_for_shape());
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

	static inline void
	setup_ellipse_vertex (
		Vertex* vertex, coord origin_x, coord origin_y, coord vertex_x, coord vertex_y)
	{
		assert(vertex);

		vertex->position.reset(origin_x + vertex_x, origin_y + vertex_y);
		vertex->texcoord.reset(vertex_x, vertex_y);
	}

	static void
	draw_ellipse (
		Painter* painter,
		coord x, coord y, coord width, coord height,
		float angle_from, float angle_to, coord hole_width, coord hole_height,
		uint nsegment)
	{
		assert(painter);

		Painter::Data* self = painter->self.get();

		if (!self->painting)
			invalid_state_error(__FILE__, __LINE__, "painting flag should be true.");

		if (!self->state.has_color() || width == 0 || height == 0)
			return;

		if (nsegment <= 0)
			nsegment = NSEGMENT_ELLIPSE;

		float from         = angle_from / 360.f;
		float to           = angle_to   / 360.f;
		bool hole          = hole_width != 0 && hole_height != 0;
		coord hole_x       = x + (width  - hole_width)  / 2;
		coord hole_y       = y + (height - hole_height) / 2;
		int npoint         = nsegment + 1;
		size_t nvertices   = hole ? npoint * 2 : npoint + 1;
		GLenum modes[]     =
		{
			(GLenum) (hole ? GL_TRIANGLE_STRIP : GL_TRIANGLE_FAN),
			GL_LINE_LOOP
		};

		std::unique_ptr<uint[]> indices(new uint[nvertices]);
		for (size_t i = 0; i < nvertices; ++i)
			indices[i] = (int) i;

		std::unique_ptr<Vertex[]> vertices(new Vertex[nvertices]);
		Vertex* vertex = vertices.get();

		if (!hole)
		{
			(vertex++)->position.reset(
				x + width  / 2,
				y + height / 2);
		}

		for (uint seg = 0; seg <= nsegment; ++seg)
		{
			float pos    = (float) seg / (float) nsegment;
			float radian = (from + (to - from) * pos) * PI_2;
			float cos_   = (cos(radian)  + 1) / 2.;
			float sin_   = (-sin(radian) + 1) / 2.;

			if (hole)
			{
				setup_ellipse_vertex(
					vertex++,
					hole_x,
					hole_y,
					hole_width  * cos_,
					hole_height * sin_);
			}

			setup_ellipse_vertex(
				vertex++,
				x,
				y,
				width  * cos_,
				height * sin_);
		}

		Color color;
		for (int type = COLOR_TYPE_BEGIN; type < COLOR_TYPE_END; ++type)
		{
			if (!self->get_color(&color, (ColorType) type))
				continue;

			self->draw_shape(
				modes[type], vertices.get(), nvertices, indices.get(), nvertices, color,
				get_default_shader_for_shape());
		}
	}

	void
	Painter::ellipse (
		coord x, coord y, coord width, coord height,
		float angle_from, float angle_to, const Point& hole_size, uint nsegment)
	{
		draw_ellipse(
			this, x, y, width, height,
			angle_from, angle_to, hole_size.x, hole_size.y, nsegment);
	}

	void
	Painter::ellipse (
		const Bounds& bounds,
		float angle_from, float angle_to, const Point& hole_size, uint nsegment)
	{
		ellipse(
			bounds.x, bounds.y, bounds.width, bounds.height,
			angle_from, angle_to, hole_size, nsegment);
	}

	void
	Painter::ellipse (
		const Point& center, coord radius,
		float angle_from, float angle_to, coord hole_radius, uint nsegment)
	{
		draw_ellipse(
			this, center.x - radius, center.y - radius, radius * 2, radius * 2,
			angle_from, angle_to, hole_radius * 2, hole_radius * 2, nsegment);
	}

	static void
	draw_image (
		Painter* painter, const Image& image,
		coord src_x, coord src_y, coord src_w, coord src_h,
		coord dst_x, coord dst_y, coord dst_w, coord dst_h,
		bool nostroke = false, const Shader* shader = NULL)
	{
		static const GLenum MODES[] = {GL_TRIANGLE_FAN, GL_LINE_LOOP};
		static const uint INDICES[] = {0, 1, 2, 3};

		assert(painter && image);

		Painter::Data* self = painter->self.get();

		if (!self->painting)
			invalid_state_error(__FILE__, __LINE__, "painting flag should be true.");

		if (!self->state.has_color())
			return;

		const Texture& texture = Image_get_texture(image);
		if (!texture)
			invalid_state_error(__FILE__, __LINE__);

		float density = image.pixel_density();
		src_x *= density;
		src_y *= density;
		src_w *= density;
		src_h *= density;

		coord src_x2 = src_x + src_w;
		coord src_y2 = src_y + src_h;
		coord dst_x2 = dst_x + dst_w;
		coord dst_y2 = dst_y + dst_h;

		Vertex vertices[4];
		vertices[0].reset(dst_x,  dst_y,  src_x,  src_y);
		vertices[1].reset(dst_x,  dst_y2, src_x,  src_y2);
		vertices[2].reset(dst_x2, dst_y2, src_x2, src_y2);
		vertices[3].reset(dst_x2, dst_y,  src_x2, src_y);

		TextureInfo texinfo(texture, src_x, src_y, src_x2, src_y2);

		if (!shader)
			shader = &get_default_shader_for_color_texture();

		Color color;
		for (int type = COLOR_TYPE_BEGIN; type < COLOR_TYPE_END; ++type)
		{
			if (
				(nostroke && type == STROKE) ||
				!self->get_color(&color, (ColorType) type))
			{
				continue;
			}

			self->draw_shape(
				MODES[type], vertices, 4, INDICES, 4, color, *shader, &texinfo);
		}
	}

	void
	Painter::image (const Image& image_, coord x, coord y)
	{
		if (!image_)
			argument_error(__FILE__, __LINE__);

		draw_image(
			this, image_,
			0, 0, image_.width(), image_.height(),
			x, y, image_.width(), image_.height());
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

		draw_image(
			this, image_,
			0, 0, image_.width(), image_.height(),
			x, y, width,          height);
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
		coord src_x, coord src_y, coord src_width, coord src_height,
		coord dst_x, coord dst_y)
	{
		if (!image_)
			argument_error(__FILE__, __LINE__);

		draw_image(
			this, image_,
			src_x, src_y, src_width,      src_height,
			dst_x, dst_y, image_.width(), image_.height());
	}

	void
	Painter::image (
		const Image& image_, const Bounds& src_bounds, const Point& dst_position)
	{
		image(
			image_,
			src_bounds.x, src_bounds.y, src_bounds.width, src_bounds.height,
			dst_position.x, dst_position.y);
	}

	void
	Painter::image (
		const Image& image_,
		coord src_x, coord src_y, coord src_width, coord src_height,
		coord dst_x, coord dst_y, coord dst_width, coord dst_height)
	{
		if (!image_)
			argument_error(__FILE__, __LINE__);

		draw_image(
			this, image_,
			src_x, src_y, src_width, src_height,
			dst_x, dst_y, dst_width, dst_height);
	}

	void
	Painter::image (
		const Image& image_, const Bounds& src_bounds, const Bounds& dst_bounds)
	{
		image(
			image_,
			src_bounds.x, src_bounds.y, src_bounds.width, src_bounds.height,
			dst_bounds.x, dst_bounds.y, dst_bounds.width, dst_bounds.height);
	}

	static void
	debug_draw_text (
		Painter* painter, const Font& font,
		coord x, coord y, coord str_width, coord str_height)
	{
		save_image(painter->self->text_image, "/tmp/font.png");

		painter->push_state();
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
		painter->pop_state();
	}

	static void
	draw_text (
		Painter* painter, const Font& font,
		const char* str, coord x, coord y, coord width = 0, coord height = 0)
	{
		assert(painter && font && str && *str != '\0');

		Painter::Data* self = painter->self.get();

		if (!self->painting)
			invalid_state_error(__FILE__, __LINE__, "painting flag should be true.");

		if (!self->state.has_color())
			return;

		float density = self->pixel_density;
		coord str_w = Font_get_width(font, density, str);
		coord str_h = Font_get_height(font, density);
		int tex_w   = ceil(str_w);
		int tex_h   = ceil(str_h);
		const Texture& texture = Image_get_texture(self->text_image);
		if (
			texture.width()  < tex_w ||
			texture.height() < tex_h ||
			self->text_image.pixel_density() != density)
		{
			int bmp_w = std::max(texture.width(),  tex_w);
			int bmp_h = std::max(texture.height(), tex_h);
			self->text_image = Image(Bitmap(bmp_w, bmp_h, ALPHA), density);
		}

		if (!self->text_image)
			invalid_state_error(__FILE__, __LINE__);

		assert(self->text_image.pixel_density() == density);

		Bitmap_draw_string(
			&self->text_image.bitmap(), Font_get_raw(font, density), str, 0, 0);

		str_w /= density;
		str_h /= density;
		if (width  == 0) width  = str_w;
		if (height == 0) height = str_h;

		draw_image(
			painter, self->text_image,
			0, 0, str_w, str_h,
			x, y, str_w, str_h,
			true, &get_default_shader_for_alpha_texture());

		#if 0
			debug_draw_text(painter, font, x, y, str_w / density, str_h / density);
		#endif
	}

	void
	Painter::text (const char* str, coord x, coord y)
	{
		if (!str)
			argument_error(__FILE__, __LINE__);

		if (*str == '\0') return;

		const Font& font = self->state.font;
		if (!font)
			invalid_state_error(__FILE__, __LINE__);

		draw_text(this, font, str, x, y);
	}

	void
	Painter::text (const char* str, const Point& position)
	{
		text(str, position.x, position.y);
	}

	void
	Painter::text (const char* str, coord x, coord y, coord width, coord height)
	{
		if (!str)
			argument_error(__FILE__, __LINE__);

		if (*str == '\0' || width == 0 || height == 0) return;

		const Font& font = self->state.font;
		if (!font)
			invalid_state_error(__FILE__, __LINE__);

		draw_text(this, font, str, x, y, width, height);
	}

	void
	Painter::text (const char* str, const Bounds& bounds)
	{
		text(str, bounds.x, bounds.y, bounds.width, bounds.height);
	}

	void
	Painter::polygon (const Polygon& poly, coord x, coord y)
	{
		std::vector<BoostPolygon> polygons;
		Polygon_get_boost_polygons(poly).get(polygons);

		for (const auto& polygon : polygons)
		{
			std::vector<Point> points;
			for (const auto& point : polygon)
				points.emplace_back(Point(x + point.x(), y + point.y()));

			line(&points[0], points.size());
		}
	}

	void
	Painter::polygon (const Polygon& poly, const Point& position)
	{
		polygon(poly, position.x, position.y);
	}

	void
	Painter::set_background (
		float red, float green, float blue, float alpha, bool clear)
	{
		set_background(Color(red, green, blue, alpha), clear);
	}

	void
	Painter::set_background (const Color& color, bool clear)
	{
		self->state.background = color;

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
		return self->state.background;
	}

	void
	Painter::set_fill (float red, float green, float blue, float alpha)
	{
		set_fill(Color(red, green, blue, alpha));
	}

	void
	Painter::set_fill (const Color& color)
	{
		self->state.colors[FILL] = color;
	}

	void
	Painter::no_fill ()
	{
		self->state.colors[FILL].alpha = 0;
	}

	const Color&
	Painter::fill () const
	{
		return self->state.colors[FILL];
	}

	void
	Painter::set_stroke (float red, float green, float blue, float alpha)
	{
		set_stroke(Color(red, green, blue, alpha));
	}

	void
	Painter::set_stroke (const Color& color)
	{
		self->state.colors[STROKE] = color;
	}

	void
	Painter::no_stroke ()
	{
		self->state.colors[STROKE].alpha = 0;
	}

	const Color&
	Painter::stroke () const
	{
		return self->state.colors[STROKE];
	}

	void
	Painter::set_shader (const Shader& shader)
	{
		self->state.shader = shader;
	}

	void
	Painter::no_shader ()
	{
		self->state.shader = Shader();
	}

	const Shader&
	Painter::shader () const
	{
		return self->state.shader;
	}

	void
	Painter::set_clip (coord x, coord y, coord width, coord height)
	{
		set_clip(Bounds(x, y, width, height));
	}

	void
	Painter::set_clip (const Bounds& bounds)
	{
		self->state.clip = bounds;
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
		return self->state.clip;
	}

	void
	Painter::set_font (const char* name, coord size)
	{
		set_font(Font(name, size));
	}

	void
	Painter::set_font (const Font& font)
	{
		self->state.font = font;
	}

	const Font&
	Painter::font () const
	{
		return self->state.font;
	}

	void
	Painter::push_state ()
	{
		self->state_stack.push_back(self->state);
	}

	void
	Painter::pop_state ()
	{
		if (self->state_stack.empty())
			invalid_state_error(__FILE__, __LINE__, "state stack underflow.");

		self->state = self->state_stack.back();
		self->state_stack.pop_back();
		self->update_clip();
	}

	void
	Painter::translate (coord x, coord y, coord z)
	{
		self->position_matrix.translate(x, y, z);
	}

	void
	Painter::translate (const Point& value)
	{
		self->position_matrix.translate(value);
	}

	void
	Painter::scale (coord x, coord y, coord z)
	{
		self->position_matrix.scale(x, y, z);
	}

	void
	Painter::scale (const Point& value)
	{
		self->position_matrix.scale(value);
	}

	void
	Painter::rotate (float degree, coord x, coord y, coord z)
	{
		self->position_matrix.rotate(degree, x, y, z);
	}

	void
	Painter::rotate (float angle, const Point& normalized_axis)
	{
		self->position_matrix.rotate(angle, normalized_axis);
	}

	void
	Painter::set_matrix (float value)
	{
		self->position_matrix.reset(value);
	}

	void
	Painter::set_matrix (
		float a1, float a2, float a3, float a4,
		float b1, float b2, float b3, float b4,
		float c1, float c2, float c3, float c4,
		float d1, float d2, float d3, float d4)
	{
		self->position_matrix.reset(
			a1, a2, a3, a4,
			b1, b2, b3, b4,
			c1, c2, c3, c4,
			d1, d2, d3, d4);
	}

	void
	Painter::set_matrix (const coord* elements, size_t size)
	{
		self->position_matrix.reset(elements, size);
	}

	void
	Painter::set_matrix (const Matrix& matrix)
	{
		self->position_matrix = matrix;
	}

	const Matrix&
	Painter::matrix () const
	{
		return self->position_matrix;
	}

	void
	Painter::push_matrix ()
	{
		self->position_matrix_stack.push_back(self->position_matrix);
	}

	void
	Painter::pop_matrix ()
	{
		if (self->position_matrix_stack.empty())
			invalid_state_error(__FILE__, __LINE__, "matrix stack underflow.");

		self->position_matrix = self->position_matrix_stack.back();
		self->position_matrix_stack.pop_back();
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
