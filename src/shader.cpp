#include "rays/shader.h"


#ifndef IOS
#define USE_SHADER
#endif


#ifdef USE_SHADER


#include <memory>
#include "rays/exception.h"


namespace Rays
{


	struct Shader::Data
	{

		int id;

		Data ()
		:	id(-1)
		{
		}

		~Data ()
		{
			clear();
		}

		void clear ()
		{
			if (id >= 0) glDeleteShader((GLuint) id);

			id = -1;
		}

		bool is_valid () const
		{
			return id >= 0;
		}

	};// Shader::Data


	void
	compile_shader (Shader* this_, const char* source)
	{
		if (!this_ || !source)
			argument_error(__FILE__, __LINE__);

		Shader::Data* self = this_->self.get();
		if (self->is_valid())
			invalid_state_error(__FILE__, __LINE__);

		self->id = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(self->id, 1, &source, NULL);
		glCompileShader(self->id);

		GLint status = GL_FALSE;
		glGetShaderiv(self->id, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE)
		{
			int len = 0;
			glGetShaderiv(self->id, GL_INFO_LOG_LENGTH, &len);

			std::unique_ptr<char[]> buffer(new char[len]);
			int written = 0;
			glGetShaderInfoLog(self->id, len, &written, &buffer[0]);

			opengl_error(__FILE__, __LINE__, &buffer[0]);
		}

		check_error(__FILE__, __LINE__);
	}


	Shader::Shader ()
	{
	}

	Shader::Shader (const char* source)
	{
		compile_shader(this, source);
	}

	Shader::~Shader ()
	{
	}

	GLuint
	Shader::id () const
	{
		return self->id;
	}

	bool
	Shader::operator == (const Shader& rhs) const
	{
		return self->id == rhs.self->id;
	}

	bool
	Shader::operator != (const Shader& rhs) const
	{
		return !operator==(rhs);
	}

	Shader::operator bool () const
	{
		return self->is_valid();
	}

	bool
	Shader::operator ! () const
	{
		return !operator bool();
	}


}// Rays


#else// USE_SHADER


#include "rays/exception.h"


namespace Rays
{


	struct Shader::Data {};


	Shader::Shader ()
	{
	}

	Shader::Shader (const char* source)
	{
		not_implemented_error(__FILE__, __LINE__);
	}

	Shader::~Shader ()
	{
	}

	GLuint
	Shader::id () const
	{
		return 0;
	}

	bool
	Shader::operator == (const Shader& rhs) const
	{
		return false;
	}

	bool
	Shader::operator != (const Shader& rhs) const
	{
		return !operator==(rhs);
	}

	Shader::operator bool () const
	{
		return false;
	}

	bool
	Shader::operator ! () const
	{
		return !operator bool();
	}


}// Rays


#endif// USE_SHADER
