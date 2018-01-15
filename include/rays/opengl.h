// -*- c++ -*-
#pragma once
#ifndef __RAYS_OPENGL_H__
#define __RAYS_OPENGL_H__


#if defined(OSX)
	#include <OpenGL/gl.h>
	#include <OpenGL/glext.h>
#elif defined(IOS)
	#include <OpenGLES/ES3/gl.h>
	#include <OpenGLES/ES3/glext.h>
#elif defined(WIN32)
	#include <GL/gl.h>
	#include <GL/glext.h>
#endif


namespace Rays
{


	typedef void* Context;


	Context get_offscreen_context ();


	GLenum get_error ();

	bool    no_error ();

	bool    is_error (GLenum err);

	void check_error(const char* file, int line);

	void clear_error ();


}// Rays


#endif//EOH
