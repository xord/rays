// -*- c++ -*-
#pragma once
#ifndef __RAYS_SRC_SHADER_H__
#define __RAYS_SRC_SHADER_H__


#include <rays/shader.h>


namespace Rays
{


	class ShaderSource;
	class ShaderProgram;


	struct ShaderBuiltinVariableNames
	{

		String attribute_position;
		String attribute_texcoord;
		String attribute_color;

		String varying_position;
		String varying_texcoord;
		String varying_color;

		String uniform_texture;
		String uniform_position_matrix;
		String uniform_texcoord_matrix;
		String uniform_texcoord_min;
		String uniform_texcoord_max;

		ShaderBuiltinVariableNames (
			const char* attribute_position,
			const char* attribute_texcoord,
			const char* attribute_color,
			const char* varying_position,
			const char* varying_texcoord,
			const char* varying_color,
			const char* uniform_texture,
			const char* uniform_position_matrix,
			const char* uniform_texcoord_matrix,
			const char* uniform_texcoord_min,
			const char* uniform_texcoord_max);

	};// ShaderBuiltinVariableNames


	const ShaderProgram* Shader_get_program (const Shader& shader);

	const ShaderBuiltinVariableNames& Shader_get_builtin_variable_names (
		const Shader& shader);

	const Shader& Shader_get_default_shader_for_shape ();

	const Shader& Shader_get_default_shader_for_texture ();


	const ShaderBuiltinVariableNames& ShaderEnv_get_builtin_variable_names (
		const ShaderEnv& env);

	const ShaderSource& ShaderEnv_get_default_vertex_shader_source (
		ShaderEnv* env);


}// Rays


#endif//EOH
