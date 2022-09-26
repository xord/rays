// -*- c++ -*-
#pragma once
#ifndef __RAYS_SRC_SHADER_H__
#define __RAYS_SRC_SHADER_H__


#include <rays/shader.h>


namespace Rays
{


	class ShaderProgram;


	struct ShaderBuiltinVariableNames
	{

		String attribute_position;
		String attribute_texcoord;
		String attribute_color;

		String varying_position;
		String varying_texcoord;
		String varying_color;

		String uniform_position_matrix;
		String uniform_texcoord_matrix;
		String uniform_texture;

		ShaderBuiltinVariableNames (
			const char* attribute_position      = "a_Position",
			const char* attribute_texcoord      = "a_TexCoord",
			const char* attribute_color         = "a_Color",
			const char* varying_position        = "v_Position",
			const char* varying_texcoord        = "v_TexCoord",
			const char* varying_color           = "v_Color",
			const char* uniform_position_matrix = "u_PositionMatrix",
			const char* uniform_texcoord_matrix = "u_TexCoordMatrix",
			const char* uniform_texture         = "u_Texture");

	};// ShaderBuiltinVariableNames


	const ShaderProgram* Shader_get_program (const Shader& shader);


	void Shader_set_builtin_variable_names (
		const ShaderBuiltinVariableNames& names);

	const ShaderBuiltinVariableNames& Shader_get_builtin_variable_names ();

	const Shader& Shader_get_default_shader_for_shape ();

	const Shader& Shader_get_default_shader_for_texture ();

}// Rays


#endif//EOH
