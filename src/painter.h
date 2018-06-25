// -*- c++ -*-
#pragma once
#ifndef __RAYS_SRC_PAINTER_H__
#define __RAYS_SRC_PAINTER_H__


#include <rays/opengl.h>
#include <rays/painter.h>


namespace Rays
{


	class ShaderSource;


	const ShaderSource& Painter_get_vertex_shader_source ();

	const ShaderSource& Painter_get_fragment_shader_shared_source ();

	void Painter_fill_polygon (
		Painter* painter, GLenum mode, const Color& color,
		const Coord3* points,         size_t npoints,
		const uint*   indices = NULL, size_t nindices = 0);


}// Rays


#endif//EOH
