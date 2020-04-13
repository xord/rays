// -*- c++ -*-
#pragma once
#ifndef __RAYS_DEFS_H__
#define __RAYS_DEFS_H__


#define GLM_FORCE_CXX03


#include <xot/defs.h>
#include <xot/string.h>


namespace Rays
{


	using namespace Xot::Types;

	using Xot::String;


	typedef float coord;


	enum CapType
	{

		CAP_BUTT = 0,

		CAP_ROUND,

		CAP_SQUARE,

		CAP_MAX,

		CAP_DEFAULT = CAP_BUTT

	};// CapType


	enum JoinType
	{

		JOIN_MITER = 0,

		JOIN_ROUND,

		JOIN_SQUARE,

		JOIN_MAX,

		JOIN_DEFAULT             = JOIN_MITER,
		JOIN_DEFAULT_MITER_LIMIT = 2

	};// JoinType


}// Rays


#endif//EOH
