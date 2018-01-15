// -*- objc -*-
#include "../opengl.h"


#import <AppKit/AppKit.h>


namespace Rays
{


	void
	OpenGL_set_context (Context context)
	{
		NSOpenGLContext* c = (NSOpenGLContext*) context;
		[c makeCurrentContext];
	}

	Context
	OpenGL_get_context ()
	{
		return [NSOpenGLContext currentContext];
	}


	Context
	get_offscreen_context ()
	{
		static Context context = NULL;
		if (!context)
		{
			NSOpenGLPixelFormat* pf  = [NSOpenGLView defaultPixelFormat];
			context = [[NSOpenGLContext alloc] initWithFormat: pf shareContext: nil];
		}
		return context;
	}


}// Rays
