#include "shader.h"


#include "rays/exception.h"
#include "opengl.h"
#include "image.h"
#include "shader_program.h"
#include "shader_source.h"


namespace Rays
{


	static const ShaderSource&
	get_vertex_shader_source ()
	{
		static const ShaderSource SOURCE(
			GL_VERTEX_SHADER,
			"attribute vec3 "  ATTRIB_POSITION ";"
			"varying   vec4 " VARYING_POSITION ";"
			"uniform   mat4 " UNIFORM_POSITION_MATRIX ";"
			"attribute vec3 "  ATTRIB_TEXCOORD ";"
			"varying   vec4 " VARYING_TEXCOORD ";"
			"uniform   mat4 " UNIFORM_TEXCOORD_MATRIX ";"
			"attribute vec4 "  ATTRIB_COLOR ";"
			"varying   vec4 " VARYING_COLOR ";"
			"void main ()"
			"{"
			"  vec4 pos           = vec4(" ATTRIB_POSITION ", 1.0);"
			"  vec4 texcoord      = vec4(" ATTRIB_TEXCOORD ", 1.0);"
			   VARYING_POSITION " = pos;"
			   VARYING_TEXCOORD " = " UNIFORM_TEXCOORD_MATRIX " * texcoord;"
			   VARYING_COLOR    " = " ATTRIB_COLOR ";"
			"  gl_Position        = " UNIFORM_POSITION_MATRIX " * pos;"
			"}");
		return SOURCE;
	}

	static ShaderSource
	make_fragment_shader_source (const char* source)
	{
		static String SHARED_HEADER =
			"#ifdef GL_ES\n"
			"precision mediump float;\n"
			"#endif\n"
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
			"}\n";
		return ShaderSource(GL_FRAGMENT_SHADER, SHARED_HEADER + source);
	}


	struct Shader::Data
	{

		std::unique_ptr<ShaderProgram> program;

	};// Shader::Data


	const ShaderProgram*
	Shader_get_program (const Shader& shader)
	{
		return shader.self->program ? shader.self->program.get() : NULL;
	}


	Shader::Shader (const char* source)
	{
		if (!source) return;

		self->program.reset(new ShaderProgram(
			get_vertex_shader_source(),
			make_fragment_shader_source(source)));
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


}// Rays
