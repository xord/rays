// -*- mode: c++ -*-
#pragma once
#ifndef __RAYS_CAMERA_H__
#define __RAYS_CAMERA_H__


#include <xot/pimpl.h>
#include <rays/defs.h>
#include <rays/image.h>


namespace Rays
{


	class Camera
	{

		typedef Camera This;

		public:

			Camera ();

			~Camera ();

			bool start ();

			void stop ();

			bool is_active () const;

			const Image* image () const;

			operator bool () const;

			bool operator ! () const;

			struct Data;

			Xot::PSharedImpl<Data> self;

	};// Camera


}// Rays


#endif//EOH
