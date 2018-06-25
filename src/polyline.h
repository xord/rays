// -*- c++ -*-
#pragma once
#ifndef __RAYS_SRC_POLYLINE_H__
#define __RAYS_SRC_POLYLINE_H__


#include <clipper.hpp>
#include <rays/polyline.h>


namespace Rays
{


	void Polyline_create (
		Polyline* polyline, const ClipperLib::Path& path, bool loop,
		bool reverse = false);

	void Polyline_get_path (
		ClipperLib::Path* path, const Polyline& polyline,
		bool reverse = false);


}// Rays


#endif//EOH
