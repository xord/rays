#include "rays/opengl.h"


#include "rays/exception.h"


namespace Rays
{


	GLenum
	get_error ()
	{
		return glGetError();
	}

	bool
	no_error ()
	{
		return get_error() == GL_NO_ERROR;
	}

	bool
	is_error (GLenum err)
	{
		return get_error() == err;
	}

	static String
	get_error_name (GLenum error)
	{
		switch (error)
		{
			case GL_NO_ERROR:          return "GL_NO_ERROR";
			case GL_INVALID_ENUM:      return "GL_INVALID_ENUM";
			case GL_INVALID_VALUE:     return "GL_INVALID_VALUE";
			case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
		#ifndef IOS
			case GL_STACK_OVERFLOW:    return "GL_STACK_OVERFLOW";
			case GL_STACK_UNDERFLOW:   return "GL_STACK_UNDERFLOW";
		#endif
			case GL_OUT_OF_MEMORY:     return "GL_OUT_OF_MEMORY";
			case 0x506:                return "GL_INVALID_FRAMEBUFFER_OPERATION";
			case 0x8031:               return "GL_TABLE_TOO_LARGE";
		}
		return "UNKNOWN ERROR";
	}

	void
	check_error (const char* file, int line)
	{
		GLenum e = get_error();
		if (e != GL_NO_ERROR)
			opengl_error(file, line, "OpenGL error %s", get_error_name(e).c_str());
	}

	void
	clear_error ()
	{
		get_error();
	}


}// Rays
