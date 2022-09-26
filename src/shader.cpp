#include "shader.h"


#include <regex>
#include "rays/exception.h"
#include "opengl.h"
#include "image.h"
#include "shader_program.h"
#include "shader_source.h"


namespace Rays
{


	class BuiltinShader
	{

		public:

			ShaderSource make_vertex_shader_source (const char* source) const
			{
				if (source)
					return ShaderSource(GL_VERTEX_SHADER, source);
				else
				{
					if (!vertex_shader_source) vertex_shader_source =
						ShaderSource(GL_VERTEX_SHADER, make_default_vertex_shader_source());
					return vertex_shader_source;
				}
			}

			ShaderSource make_fragment_shader_source (const char* source) const
			{
				static const String SHARED_HEADER =
					"#ifdef GL_ES\n"
					"precision mediump float;\n"
					"#endif\n";
				static const std::regex HAS_PRECISION(R"(^\s*precision\s+\w+p\s+float\s*;)");

				if (std::regex_search(source, HAS_PRECISION))
					return ShaderSource(GL_FRAGMENT_SHADER, source);
				else
					return ShaderSource(GL_FRAGMENT_SHADER, SHARED_HEADER + source);
			}

			const Shader& default_shader_for_shape () const
			{
				if (!shape_shader) shape_shader = make_default_shader_for_shape();
				return shape_shader;
			}

			const Shader& default_shader_for_texture () const
			{
				if (!texture_shader) texture_shader = make_default_shader_for_texture();
				return texture_shader;
			}

			void clear ()
			{
				vertex_shader_source = ShaderSource();
				shape_shader         = Shader();
				texture_shader       = Shader();
			}

		private:

			mutable ShaderSource vertex_shader_source;

			mutable Shader shape_shader, texture_shader;

			#define A_POSITION        (bvn.attribute_position)
			#define A_TEXCOORD        (bvn.attribute_texcoord)
			#define A_COLOR           (bvn.attribute_color)
			#define V_POSITION        (bvn.varying_position)
			#define V_TEXCOORD        (bvn.varying_texcoord)
			#define V_COLOR           (bvn.varying_color)
			#define U_POSITION_MATRIX (bvn.uniform_position_matrix)
			#define U_TEXCOORD_MATRIX (bvn.uniform_texcoord_matrix)
			#define U_TEXTURE         (bvn.uniform_texture)

			String make_default_vertex_shader_source () const
			{
				const auto& bvn = Shader_get_builtin_variable_names();
				return
					"attribute vec3 " + A_POSITION + ";\n"
					"attribute vec3 " + A_TEXCOORD + ";\n"
					"attribute vec4 " + A_COLOR + ";\n"
					"varying   vec4 " + V_POSITION + ";\n"
					"varying   vec4 " + V_TEXCOORD + ";\n"
					"varying   vec4 " + V_COLOR + ";\n"
					"uniform   mat4 " + U_POSITION_MATRIX + ";\n"
					"uniform   mat4 " + U_TEXCOORD_MATRIX + ";\n"
					"void main ()\n"
					"{\n"
					"  vec4 rays__pos      = vec4(" + A_POSITION + ", 1.0);\n"
					"  vec4 rays__texcoord = vec4(" + A_TEXCOORD + ", 1.0);\n"
					"  " + V_POSITION + "  = rays__pos;\n"
					"  " + V_TEXCOORD + "  = " + U_TEXCOORD_MATRIX + " * rays__texcoord;\n"
					"  " + V_COLOR    + "  = " + A_COLOR + ";\n"
					"  gl_Position         = " + U_POSITION_MATRIX + " * rays__pos;\n"
					"}\n";
			}

			Shader make_default_shader_for_shape () const
			{
				const auto& bvn = Shader_get_builtin_variable_names();
				return Shader(
					"varying vec4 " + V_COLOR + ";\n"
					"void main ()\n"
					"{\n"
					"  gl_FragColor = " + V_COLOR + ";\n"
					"}\n");
			}

			Shader make_default_shader_for_texture () const
			{
				const auto& bvn = Shader_get_builtin_variable_names();
				return Shader(
					"varying vec4 " + V_TEXCOORD + ";\n"
					"varying vec4 " + V_COLOR + ";\n"
					"uniform sampler2D " + U_TEXTURE + ";\n"
					"void main ()\n"
					"{\n"
					"  vec4 rays__color = texture2D(" + U_TEXTURE + ", " + V_TEXCOORD + ".xy);\n"
					"  gl_FragColor     = " + V_COLOR + " * rays__color;\n"
					"}\n");
			}

			#undef A_POSITION
			#undef A_TEXCOORD
			#undef A_COLOR
			#undef V_POSITION
			#undef V_TEXCOORD
			#undef V_COLOR
			#undef U_POSITION_MATRIX
			#undef U_TEXCOORD_MATRIX
			#undef U_TEXTURE

	};// BuiltinShader


	namespace global
	{

		ShaderBuiltinVariableNames names;

		BuiltinShader builtin_shader;

	};// global


	struct Shader::Data
	{

		std::unique_ptr<ShaderProgram> program;

	};// Shader::Data


	const ShaderProgram*
	Shader_get_program (const Shader& shader)
	{
		return shader.self->program ? shader.self->program.get() : NULL;
	}

	void Shader_set_builtin_variable_names (
		const ShaderBuiltinVariableNames& names)
	{
		global::names = names;
		global::builtin_shader.clear();
	}

	const ShaderBuiltinVariableNames&
	Shader_get_builtin_variable_names ()
	{
		return global::names;
	}

	const Shader&
	Shader_get_default_shader_for_shape ()
	{
		return global::builtin_shader.default_shader_for_shape();
	}

	const Shader&
	Shader_get_default_shader_for_texture ()
	{
		return global::builtin_shader.default_shader_for_texture();
	}


	Shader::Shader (
		const char* fragment_shader_source,
		const char*   vertex_shader_source)
	{
		if (!fragment_shader_source) return;

		const auto& bs = global::builtin_shader;
		self->program.reset(new ShaderProgram(
			bs.make_vertex_shader_source(vertex_shader_source),
			bs.make_fragment_shader_source(fragment_shader_source)));
	}

	Shader::~Shader ()
	{
	}

	void
	Shader::set_uniform (const char* name, int arg1)
	{
		if (!self->program)
			invalid_state_error(__FILE__, __LINE__);

		self->program->set_uniform(name, arg1);
	}

	void
	Shader::set_uniform (const char* name, int arg1, int arg2)
	{
		if (!self->program)
			invalid_state_error(__FILE__, __LINE__);

		self->program->set_uniform(name, arg1, arg2);
	}

	void
	Shader::set_uniform (const char* name, int arg1, int arg2, int arg3)
	{
		if (!self->program)
			invalid_state_error(__FILE__, __LINE__);

		self->program->set_uniform(name, arg1, arg2, arg3);
	}

	void
	Shader::set_uniform (const char* name, int arg1, int arg2, int arg3, int arg4)
	{
		if (!self->program)
			invalid_state_error(__FILE__, __LINE__);

		self->program->set_uniform(name, arg1, arg2, arg3, arg4);
	}

	void
	Shader::set_uniform (const char* name, const int* args, size_t size)
	{
		if (!self->program)
			invalid_state_error(__FILE__, __LINE__);

		self->program->set_uniform(name, args, size);
	}

	void
	Shader::set_uniform (const char* name, float arg1)
	{
		if (!self->program)
			invalid_state_error(__FILE__, __LINE__);

		self->program->set_uniform(name, arg1);
	}

	void
	Shader::set_uniform (const char* name, float arg1, float arg2)
	{
		if (!self->program)
			invalid_state_error(__FILE__, __LINE__);

		self->program->set_uniform(name, arg1, arg2);
	}

	void
	Shader::set_uniform (const char* name, float arg1, float arg2, float arg3)
	{
		if (!self->program)
			invalid_state_error(__FILE__, __LINE__);

		self->program->set_uniform(name, arg1, arg2, arg3);
	}

	void
	Shader::set_uniform (
		const char* name, float arg1, float arg2, float arg3, float arg4)
	{
		if (!self->program)
			invalid_state_error(__FILE__, __LINE__);

		self->program->set_uniform(name, arg1, arg2, arg3, arg4);
	}

	void
	Shader::set_uniform (const char* name, const float* args, size_t size)
	{
		if (!self->program)
			invalid_state_error(__FILE__, __LINE__);

		self->program->set_uniform(name, args, size);
	}

	void
	Shader::set_uniform (const char* name, const Coord2& vec2)
	{
		if (!self->program)
			invalid_state_error(__FILE__, __LINE__);

		self->program->set_uniform(name, vec2);
	}

	void
	Shader::set_uniform (const char* name, const Coord3& vec3)
	{
		if (!self->program)
			invalid_state_error(__FILE__, __LINE__);

		self->program->set_uniform(name, vec3);
	}

	void
	Shader::set_uniform (const char* name, const Coord4& vec4)
	{
		if (!self->program)
			invalid_state_error(__FILE__, __LINE__);

		self->program->set_uniform(name, vec4);
	}

	void
	Shader::set_uniform (const char* name, const Image& texture)
	{
		if (!self->program)
			invalid_state_error(__FILE__, __LINE__);

		self->program->set_uniform(name, Image_get_texture(texture));
	}

	Shader::operator bool () const
	{
		return self->program && *self->program;
	}

	bool
	Shader::operator ! () const
	{
		return !operator bool();
	}

	bool
	operator == (const Shader& lhs, const Shader& rhs)
	{
		return (!lhs && !rhs) || lhs.self->program == rhs.self->program;
	}

	bool
	operator != (const Shader& lhs, const Shader& rhs)
	{
		return !operator==(lhs, rhs);
	}


	ShaderBuiltinVariableNames::ShaderBuiltinVariableNames(
		const char* a_position,
		const char* a_texcoord,
		const char* a_color,
		const char* v_position,
		const char* v_texcoord,
		const char* v_color,
		const char* u_position_matrix,
		const char* u_texcoord_matrix,
		const char* u_texture)
	:	attribute_position(a_position),
		attribute_texcoord(a_texcoord),
		attribute_color(a_color),
		varying_position(v_position),
		varying_texcoord(v_texcoord),
		varying_color(v_color),
		uniform_position_matrix(u_position_matrix),
		uniform_texcoord_matrix(u_texcoord_matrix),
		uniform_texture(u_texture)
	{
		if (
			attribute_position     .empty() ||
			attribute_texcoord     .empty() ||
			attribute_color        .empty() ||
			varying_position       .empty() ||
			varying_texcoord       .empty() ||
			varying_color          .empty() ||
			uniform_position_matrix.empty() ||
			uniform_texcoord_matrix.empty() ||
			uniform_texture        .empty())
		{
			argument_error(__FILE__, __LINE__);
		}
	}


}// Rays
