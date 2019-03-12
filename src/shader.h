// -*- c++ -*-
#pragma once
#ifndef __RAYS_SRC_SHADER_H__
#define __RAYS_SRC_SHADER_H__


#include <rays/shader.h>


namespace Rays
{


	#define  ATTRIB_POSITION        "a_Position"
	#define VARYING_POSITION        "v_Position"
	#define UNIFORM_POSITION_MATRIX "u_PositionMatrix"
	#define  ATTRIB_TEXCOORD        "a_TexCoord"
	#define VARYING_TEXCOORD        "v_TexCoord"
	#define UNIFORM_TEXCOORD_MIN    "u_TexCoordMin"
	#define UNIFORM_TEXCOORD_MAX    "u_TexCoordMax"
	#define UNIFORM_TEXCOORD_MATRIX "u_TexCoordMatrix"
	#define  ATTRIB_COLOR           "a_Color"
	#define VARYING_COLOR           "v_Color"
	//#define UNIFORM_COLOR_MATRIX    "u_ColorMatrix"
	#define UNIFORM_TEXTURE         "u_Texture"
	#define UNIFORM_TEXTURE_SIZE    "u_TextureSize"


	class ShaderProgram;


	const ShaderProgram* Shader_get_program (const Shader& shader);


}// Rays


#endif//EOH
