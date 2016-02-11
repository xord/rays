// -*- objc -*-
#include "rays/rays.h"


#import <Foundation/Foundation.h>
#include "rays/exception.h"


namespace Rays
{


	namespace global
	{


		static NSAutoreleasePool* pool = nil;


	}// global


	void
	init ()
	{
		if (global::pool)
			rays_error(__FILE__, __LINE__, "Rays::init(): already initialized.");

		global::pool = [[NSAutoreleasePool alloc] init];

		void init_offscreen_context ();
		init_offscreen_context();
	}

	void
	fin ()
	{
		if (!global::pool)
			rays_error(__FILE__, __LINE__, "Rays::fin(): not initialized.");

		[global::pool release];
		global::pool = nil;
	}


}// Rays
