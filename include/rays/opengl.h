// -*- c++ -*-
#pragma once
#ifndef __RAYS_OPENGL_H__
#define __RAYS_OPENGL_H__


#if defined(OSX)
	#include <OpenGL/gl.h>
	#include <OpenGL/glext.h>
#elif defined(IOS)
	#include <OpenGLES/ES1/gl.h>
	#include <OpenGLES/ES1/glext.h>
#elif defined(WIN32)
	#include <GL/gl.h>
	#include <GL/glext.h>
#endif


namespace Rays
{


	GLenum get_error ();

	bool no_error ();

	bool is_error (GLenum err);

	void check_error(const char* file, int line);

	void clear_error ();


	void init_offscreen_context ();


}// Rays


#ifdef IOS

	#define glGenFramebuffers        glGenFramebuffersOES
	#define glGenRenderbuffers       glGenRenderbuffersOES
	#define glDeleteFramebuffers     glDeleteFramebuffersOES
	#define glDeleteRenderbuffers    glDeleteRenderbuffersOES
	#define glBindFramebuffer        glBindFramebufferOES
	#define glBindRenderbuffer       glBindRenderbufferOES
	#define glCheckFramebufferStatus glCheckFramebufferStatusOES
	#define glFramebufferTexture2D   glFramebufferTexture2DOES
	#define glRenderbufferStorage    glRenderbufferStorageOES
	#define glOrtho                  glOrthof

	#define GL_UNSIGNED_INT             GL_UNSIGNED_INT_OES
	#define GL_FRAMEBUFFER              GL_FRAMEBUFFER_OES
	#define GL_RENDERBUFFER             GL_RENDERBUFFER_OES
	#define GL_DRAW_FRAMEBUFFER         GL_DRAW_FRAMEBUFFER_APPLE
	#define GL_READ_FRAMEBUFFER         GL_READ_FRAMEBUFFER_APPLE
	#define GL_DRAW_FRAMEBUFFER_BINDING GL_DRAW_FRAMEBUFFER_BINDING_APPLE
	#define GL_READ_FRAMEBUFFER_BINDING GL_READ_FRAMEBUFFER_BINDING_APPLE
	#define GL_COLOR_ATTACHMENT0        GL_COLOR_ATTACHMENT0_OES
	#define GL_DEPTH_ATTACHMENT         GL_DEPTH_ATTACHMENT_OES
	#define GL_DEPTH_COMPONENT24        GL_DEPTH_COMPONENT24_OES
	#define GL_FRAMEBUFFER_COMPLETE     GL_FRAMEBUFFER_COMPLETE_OES

#endif// IOS


#endif//EOH
