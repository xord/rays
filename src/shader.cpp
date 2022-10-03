#include "shader.h"


#include <regex>
#include "rays/exception.h"
#include "opengl.h"
#include "image.h"
#include "shader_program.h"
#include "shader_source.h"


namespace Rays
{


	#define A_POSITION        (names.attribute_position_names[0])
	#define A_TEXCOORD        (names.attribute_texcoord_names[0])
	#define A_COLOR           (names.attribute_color_names[0])
	#define V_POSITION        (names.varying_position_names[0])
	#define V_TEXCOORD        (names.varying_texcoord_names[0])
	#define V_COLOR           (names.varying_color_names[0])
	#define U_TEXTURE         (names.uniform_texture_names[0])
	#define U_POSITION_MATRIX (names.uniform_position_matrix_names[0])
	#define U_TEXCOORD_MATRIX (names.uniform_texcoord_matrix_names[0])
	#define U_TEXCOORD_MIN    (names.uniform_texcoord_min_names[0])
	#define U_TEXCOORD_MAX    (names.uniform_texcoord_max_names[0])
	#define U_TEXCOORD_OFFSET (names.uniform_texcoord_offset_names[0])


	static ShaderEnv DEFAULT_ENV;


	struct Shader::Data
	{

		std::unique_ptr<ShaderProgram> program;

		std::unique_ptr<ShaderEnv> env;

		Data (
			const char* fragment_shader_source,
			const char*   vertex_shader_source,
			ShaderEnv* env_)
		{
			if (!fragment_shader_source) return;

			if (env_)
				this->env.reset(new ShaderEnv(*env_));

			program.reset(new ShaderProgram(
				make_vertex_shader_source(vertex_shader_source),
				make_fragment_shader_source(fragment_shader_source)));
		}

		ShaderSource make_vertex_shader_source (const char* source)
		{
			if (source)
				return ShaderSource(GL_VERTEX_SHADER, source);
			else
			{
				return ShaderEnv_get_default_vertex_shader_source(
					env ? env.get() : &DEFAULT_ENV);
			}
		}

		ShaderSource make_fragment_shader_source (const char* source)
		{
			static const String SHARED_HEADER =
				"#ifdef GL_ES\n"
				"precision mediump float;\n"
				"#endif\n";
			static const std::regex PRECISION(R"(^\s*precision\s+\w+p\s+float\s*;)");

			if (std::regex_search(source, PRECISION))
				return ShaderSource(GL_FRAGMENT_SHADER, source);
			else
				return ShaderSource(GL_FRAGMENT_SHADER, SHARED_HEADER + source);
		}

	};// Shader::Data


	static Shader
	make_default_shader_for_shape ()
	{
		const ShaderBuiltinVariableNames& names =
			ShaderEnv_get_builtin_variable_names(DEFAULT_ENV);
		return Shader(
			"varying vec4 " + V_COLOR + ";\n"
			"void main ()\n"
			"{\n"
			"  gl_FragColor = " + V_COLOR + ";\n"
			"}\n");
	}

	static Shader
	make_default_shader_for_texture ()
	{
		const ShaderBuiltinVariableNames& names =
			ShaderEnv_get_builtin_variable_names(DEFAULT_ENV);
		return Shader(
			"varying vec4 " + V_TEXCOORD + ";\n"
			"varying vec4 " + V_COLOR + ";\n"
			"uniform vec3 " + U_TEXCOORD_MIN + ";\n"
			"uniform vec3 " + U_TEXCOORD_MAX + ";\n"
			"uniform sampler2D " + U_TEXTURE + ";\n"
			"void main ()\n"
			"{\n"
			"  vec2 __min      = " + U_TEXCOORD_MIN + ".xy;\n"
			"  vec2 __len      = " + U_TEXCOORD_MAX + ".xy - __min;\n"
			"  vec2 __texcoord = mod(" + V_TEXCOORD + ".xy - __min, __len) + __min;\n"
			"  vec4 __color    = texture2D(" + U_TEXTURE + ", __texcoord);\n"
			"  gl_FragColor    = " + V_COLOR + " * __color;\n"
			"}\n");
	}

	const ShaderProgram*
	Shader_get_program (const Shader& shader)
	{
		return shader.self->program ? shader.self->program.get() : NULL;
	}

	const ShaderBuiltinVariableNames&
	Shader_get_builtin_variable_names (const Shader& shader)
	{
		return ShaderEnv_get_builtin_variable_names(
			shader.self->env ? *shader.self->env : DEFAULT_ENV);
	}

	const Shader&
	Shader_get_default_shader_for_shape ()
	{
		static const Shader SHADER = make_default_shader_for_shape();
		return SHADER;
	}

	const Shader&
	Shader_get_default_shader_for_texture ()
	{
		static const Shader SHADER = make_default_shader_for_texture();
		return SHADER;
	}


	Shader::Shader (
		const char* fragment_shader_source,
		const char*   vertex_shader_source)
	:	self(new Data(fragment_shader_source, vertex_shader_source, NULL))
	{
	}

	Shader::Shader (
		const char* fragment_shader_source,
		const char*   vertex_shader_source,
		ShaderEnv env)
	:	self(new Data(fragment_shader_source, vertex_shader_source, &env))
	{
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

	const char*
	Shader::vertex_shader_source () const
	{
		return self->program
			? self->program->vertex_shader_source().source()
			: NULL;
	}

	const char*
	Shader::fragment_shader_source () const
	{
		return self->program
			? self->program->fragment_shader_source().source()
			: NULL;
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


	struct ShaderEnv::Data
	{

		ShaderBuiltinVariableNames names;

		ShaderSource default_vertex_shader_source;

		Data (const ShaderBuiltinVariableNames& names)
		:	names(names)
		{
		}

		String make_default_vertex_shader_source_code ()
		{
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
				"  vec4 __pos      = vec4(" + A_POSITION + ", 1.0);\n"
				"  vec4 __texcoord = vec4(" + A_TEXCOORD + ", 1.0);\n"
				"  " + V_POSITION + "  = __pos;\n"
				"  " + V_TEXCOORD + "  = " + U_TEXCOORD_MATRIX + " * __texcoord;\n"
				"  " + V_COLOR    + "  = " + A_COLOR + ";\n"
				"  gl_Position         = " + U_POSITION_MATRIX + " * __pos;\n"
				"}\n";
		}

	};// ShaderEnv::Data


	const ShaderBuiltinVariableNames&
	ShaderEnv_get_builtin_variable_names (const ShaderEnv& env)
	{
		return env.self->names;
	}

	const ShaderSource&
	ShaderEnv_get_default_vertex_shader_source (ShaderEnv* env)
	{
		if (!env)
			argument_error(__FILE__, __LINE__);

		ShaderEnv::Data* self = env->self.get();

		if (!self->default_vertex_shader_source)
		{
			self->default_vertex_shader_source = ShaderSource(
				GL_VERTEX_SHADER, self->make_default_vertex_shader_source_code());
		}
		return self->default_vertex_shader_source;
	}


	ShaderEnv::ShaderEnv (
		const NameList& a_position_names,
		const NameList& a_texcoord_names,
		const NameList& a_color_names,
		const NameList& v_position_names,
		const NameList& v_texcoord_names,
		const NameList& v_color_names,
		const NameList& u_texture_names,
		const NameList& u_position_matrix_names,
		const NameList& u_texcoord_matrix_names,
		const NameList& u_texcoord_min_names,
		const NameList& u_texcoord_max_names,
		const NameList& u_texcoord_offset_names)
	:	self(new Data(ShaderBuiltinVariableNames(
			a_position_names, a_texcoord_names, a_color_names,
			v_position_names, v_texcoord_names, v_color_names,
			u_texture_names, u_position_matrix_names, u_texcoord_matrix_names,
			u_texcoord_min_names, u_texcoord_max_names, u_texcoord_offset_names)))
	{
	}


	static void
	validate_names (
		ShaderEnv::NameList* names,
		const char* for_variable,
		const char* default_name)
	{
		assert(names);

		if (names->empty()) names->emplace_back(default_name);

		for (const auto& name : *names)
		{
			if (name.empty())
			{
				argument_error(
					__FILE__, __LINE__,
					"invalid variable name '%s' for '%s'", name.c_str(), for_variable);
			}
		}
	}

	ShaderBuiltinVariableNames::ShaderBuiltinVariableNames (
		const ShaderEnv::NameList& a_position,
		const ShaderEnv::NameList& a_texcoord,
		const ShaderEnv::NameList& a_color,
		const ShaderEnv::NameList& v_position,
		const ShaderEnv::NameList& v_texcoord,
		const ShaderEnv::NameList& v_color,
		const ShaderEnv::NameList& u_texture,
		const ShaderEnv::NameList& u_position_matrix,
		const ShaderEnv::NameList& u_texcoord_matrix,
		const ShaderEnv::NameList& u_texcoord_min,
		const ShaderEnv::NameList& u_texcoord_max,
		const ShaderEnv::NameList& u_texcoord_offset)
	:	attribute_position_names(a_position),
		attribute_texcoord_names(a_texcoord),
		attribute_color_names(a_color),
		varying_position_names(v_position),
		varying_texcoord_names(v_texcoord),
		varying_color_names(v_color),
		uniform_texture_names(u_texture),
		uniform_position_matrix_names(u_position_matrix),
		uniform_texcoord_matrix_names(u_texcoord_matrix),
		uniform_texcoord_min_names(u_texcoord_min),
		uniform_texcoord_max_names(u_texcoord_max),
		uniform_texcoord_offset_names(u_texcoord_offset)
	{
		validate_names(&attribute_position_names,      "attribute_position",      "a_Position");
		validate_names(&attribute_texcoord_names,      "attribute_texcoord",      "a_TexCoord");
		validate_names(&attribute_color_names,         "attribute_color",         "a_Color");
		validate_names(&varying_position_names,        "varying_position",        "v_Position");
		validate_names(&varying_texcoord_names,        "varying_texcoord",        "v_TexCoord");
		validate_names(&varying_color_names,           "varying_color",           "v_Color");
		validate_names(&uniform_texture_names,         "uniform_texture",         "u_Texture");
		validate_names(&uniform_position_matrix_names, "uniform_position_matrix", "u_PositionMatrix");
		validate_names(&uniform_texcoord_matrix_names, "uniform_texcoord_matrix", "u_TexCoordMatrix");
		validate_names(&uniform_texcoord_min_names,    "uniform_texcoord_min",    "u_TexCoordMin");
		validate_names(&uniform_texcoord_max_names,    "uniform_texcoord_max",    "u_TexCoordMax");
		validate_names(&uniform_texcoord_offset_names, "uniform_texcoord_offset", "u_TexCoordOffset");
	}


}// Rays
