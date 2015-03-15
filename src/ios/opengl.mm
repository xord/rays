// -*- objc -*-
#include "rays/opengl.h"


#include <vector>
#import <OpenGLES/EAGL.h>


namespace Rays
{


	void
	init_offscreen_context ()
	{
		EAGLContext* c = [[EAGLContext alloc] initWithAPI: kEAGLRenderingAPIOpenGLES1];
		[EAGLContext setCurrentContext: c];
	}


}// Rays
