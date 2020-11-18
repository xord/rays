// -*- mode: c++ -*-
#pragma once
#ifndef __RAYS_SRC_OSX_BITMAP_H__
#define __RAYS_SRC_OSX_BITMAP_H__


#import <CoreGraphics/CGImage.h>
#include "../bitmap.h"


namespace Rays
{


	void Bitmap_copy_pixels (Bitmap* bitmap, CGImageRef image);


}// Rays


#endif//EOH
