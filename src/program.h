// -*- c++ -*-
#pragma once
#ifndef __RAYS_PROGRAM_H__
#define __RAYS_PROGRAM_H__


#include <xot/pimpl.h>
#include <rays/opengl.h>


namespace Rays
{


	class Shader;


	class Program
	{

		typedef Program This;

		public:

			Program ();

			~Program ();

			void attach (const Shader& shader);

			void detach (const Shader& shader);

			void set_uniform (const char* name, int arg1);

			void set_uniform (const char* name, int arg1, int arg2);

			void set_uniform (const char* name, int arg1, int arg2, int arg3);

			void set_uniform (const char* name, int arg1, int arg2, int arg3, int arg4);

			void set_uniform (const char* name, const int* args, size_t size);

			void set_uniform (const char* name, float arg1);

			void set_uniform (const char* name, float arg1, float arg2);

			void set_uniform (const char* name, float arg1, float arg2, float arg3);

			void set_uniform (const char* name, float arg1, float arg2, float arg3, float arg4);

			void set_uniform (const char* name, const float* args, size_t size);

			void push ();

			void pop ();

			GLuint id () const;

			operator bool () const;

			bool operator ! () const;

			struct Data;

			Xot::PImpl<Data, true> self;

	};// Program


}// Rays


#endif//EOH
