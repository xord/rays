// -*- objc -*-
#include "rays/opengl.h"


#import <AppKit/AppKit.h>
#include "../opengl.h"


namespace Rays
{


	void
	init_offscreen_context ()
	{
		NSOpenGLPixelFormat* pf  = [NSOpenGLView defaultPixelFormat];
		NSOpenGLContext* context =
			[[[NSOpenGLContext alloc] initWithFormat: pf shareContext: nil] autorelease];

		set_context(context);
	}

	void
	set_context (Context context)
	{
		NSOpenGLContext* c = (NSOpenGLContext*) context;
		[c makeCurrentContext];
	}

	Context
	get_context ()
	{
		return [NSOpenGLContext currentContext];
	}


}// Rays
