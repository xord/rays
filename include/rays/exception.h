// -*- c++ -*-
#pragma once
#ifndef __RAYS_EXCEPTION_H__
#define __RAYS_EXCEPTION_H__


#include <xot/exception.h>
#include <rays/defs.h>


namespace Rays
{


	class RaysError : public Xot::XotError
	{
		typedef Xot::XotError Super;
		public: RaysError (const char* str = NULL);
	};


	class OpenGLError : public RaysError
	{
		typedef RaysError Super;
		public: OpenGLError (const char* str = NULL);
	};


	namespace ErrorFunctions
	{

		using namespace Xot::ErrorFunctions;

		void rays_error   (const char* file, int line, const char* format = NULL, ...);

		void opengl_error (const char* file, int line, const char* format = NULL, ...);

	}// ErrorFunctions


	using namespace ErrorFunctions;


}// Rays


#endif//EOH
