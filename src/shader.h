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

		ShaderEnv::NameList attribute_position_names;
		ShaderEnv::NameList attribute_texcoord_names;
		ShaderEnv::NameList attribute_color_names;

		ShaderEnv::NameList varying_position_names;
		ShaderEnv::NameList varying_texcoord_names;
		ShaderEnv::NameList varying_color_names;

		ShaderEnv::NameList uniform_texture_names;
		ShaderEnv::NameList uniform_position_matrix_names;
		ShaderEnv::NameList uniform_texcoord_matrix_names;
		ShaderEnv::NameList uniform_texcoord_min_names;
		ShaderEnv::NameList uniform_texcoord_max_names;
		ShaderEnv::NameList uniform_texcoord_offset_names;

		ShaderBuiltinVariableNames (
			const ShaderEnv::NameList& attribute_position_names,
			const ShaderEnv::NameList& attribute_texcoord_names,
			const ShaderEnv::NameList& attribute_color_names,
			const ShaderEnv::NameList& varying_position_names,
			const ShaderEnv::NameList& varying_texcoord_names,
			const ShaderEnv::NameList& varying_color_names,
			const ShaderEnv::NameList& uniform_texture_names,
			const ShaderEnv::NameList& uniform_position_matrix_names,
			const ShaderEnv::NameList& uniform_texcoord_matrix_names,
			const ShaderEnv::NameList& uniform_texcoord_min_names,
			const ShaderEnv::NameList& uniform_texcoord_max_names,
			const ShaderEnv::NameList& uniform_texcoord_offset_names);

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
