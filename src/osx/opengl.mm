// -*- objc -*-
#include "rays/opengl.h"


#include <vector>
#import <Cocoa/Cocoa.h>
#import <AppKit/NSApplication.h>
#import <AppKit/NSWindow.h>
#import <AppKit/NSOpenGLView.h>
#import <OpenGL/OpenGL.h>


static NSOpenGLPixelFormat*
make_pixelformat ()
{
	static const NSOpenGLPixelFormatAttribute DEFAULT[] =
	{
		NSOpenGLPFAWindow,
		//NSOpenGLPFAAccelerated,
		NSOpenGLPFADoubleBuffer,
		//NSOpenGLPFAColorSize, 24,
		//NSOpenGLPFAAlphaSize, 8,
		NSOpenGLPFADepthSize, 24,
		//NSOpenGLPFANoRecovery,
	};
	static const size_t DEFAULT_SIZE  = sizeof(DEFAULT) / sizeof(DEFAULT[0]);

	std::vector<NSOpenGLPixelFormatAttribute> attr(
		DEFAULT, DEFAULT + DEFAULT_SIZE);
	attr.push_back(0);

	return [[[NSOpenGLPixelFormat alloc] initWithAttributes: &attr[0]] autorelease];
}


@interface OffscreenGLView : NSOpenGLView
@end// OffscreenGLView


@implementation OffscreenGLView

	- (id) init
	{
		self = [super
			initWithFrame: NSMakeRect(0, 0, 0, 0)
			pixelFormat:   make_pixelformat()];
		if (!self) return nil;

		[[self openGLContext] makeCurrentContext];
		return self;
	}

@end// OffscreenGLView


@interface OffscreenWindow : NSWindow

	{
		@private
			OffscreenGLView* view;
	}

@end// OffscreenWindow


@implementation OffscreenWindow

	- (id) init
	{
		self = [super
			initWithContentRect: NSMakeRect(0, 0, 0, 0)
			styleMask: 0
			backing: NSBackingStoreBuffered
			defer: NO];
		if (!self) return nil;

		view = [[OffscreenGLView alloc] init];
		[self setContentView: view];
		return self;
	}

	- (void) dealloc
	{
		if (view) [view release];
		[super dealloc];
	}

@end// OffscreenWindow


namespace Rays
{


	void
	init_offscreen_context ()
	{
		[NSApplication sharedApplication];
		[[[OffscreenWindow alloc] init] autorelease];
	}


}// Rays
