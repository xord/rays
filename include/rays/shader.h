// -*- c++ -*-
#pragma once
#ifndef __RAYS_SHADER_H__
#define __RAYS_SHADER_H__


#include <xot/pimpl.h>
#include <rays/defs.h>
#include <rays/coord.h>


namespace Rays
{


	class Image;
	class ShaderEnv;


	class Shader
	{

		typedef Shader This;

		public:

			Shader (
				const char* fragment_shader_source = NULL,
				const char*   vertex_shader_source = NULL);

			Shader (
				const char* fragment_shader_source,
				const char*   vertex_shader_source,
				ShaderEnv env);

			~Shader ();

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

			void set_uniform (const char* name, const Coord2& vec2);

			void set_uniform (const char* name, const Coord3& vec3);

			void set_uniform (const char* name, const Coord4& vec4);

			void set_uniform (const char* name, const Image& texture);

			const char*   vertex_shader_source () const;

			const char* fragment_shader_source () const;

			operator bool () const;

			bool operator ! () const;

			friend bool operator == (const This& lhs, const This& rhs);

			friend bool operator != (const This& lhs, const This& rhs);

			struct Data;

			Xot::PSharedImpl<Data> self;

	};// Shader


	class ShaderEnv
	{

		public:

			ShaderEnv (
				const char* attribute_position_name      = NULL,
				const char* attribute_texcoord_name      = NULL,
				const char* attribute_color_name         = NULL,
				const char* varying_position_name        = NULL,
				const char* varying_texcoord_name        = NULL,
				const char* varying_color_name           = NULL,
				const char* uniform_texture_name         = NULL,
				const char* uniform_position_matrix_name = NULL,
				const char* uniform_texcoord_matrix_name = NULL,
				const char* uniform_texcoord_min_name    = NULL,
				const char* uniform_texcoord_max_name    = NULL);

			struct Data;

			Xot::PSharedImpl<Data> self;

	};// ShaderEnv


}// Rays


#endif//EOH
