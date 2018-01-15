// -*- c++ -*-
#pragma once
#ifndef __RAYS_SRC_PAINTER_H__
#define __RAYS_SRC_PAINTER_H__


#include <rays/painter.h>


namespace Rays
{


	class ShaderSource;


	void Painter_set_viewport (
		Painter* painter,
		coord x, coord y, coord width, coord height, float pixel_density = 1);

	const ShaderSource& Painter_get_vertex_shader_source ();


}// Rays


#endif//EOH
