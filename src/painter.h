// -*- c++ -*-
#pragma once
#ifndef __RAYS_SRC_PAINTER_H__
#define __RAYS_SRC_PAINTER_H__


#include <rays/painter.h>


namespace Rays
{


	class ShaderSource;


	enum
	{

		NSEGMENT_ROUND   = 8,

		NSEGMENT_ELLIPSE = NSEGMENT_ROUND * 4

	};


	const ShaderSource& Painter_get_vertex_shader_source ();

	const ShaderSource& Painter_get_fragment_shader_shared_source ();


}// Rays


#endif//EOH
