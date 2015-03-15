// -*- c++ -*-
#pragma once
#ifndef __RAYS_COLOR_H__
#define __RAYS_COLOR_H__


#include <xot/util.h>
#include <rays/defs.h>
#include <rays/point.h>


namespace Rays
{


	class ColorSpace;


	struct Color
	{

		typedef Color This;

		union
		{
			struct {float red, green, blue, alpha;};
			struct {float r,   g,     b,    a;};
			float array[4];
		};

		Color (float gray = 0,                     float alpha = 1);

		Color (float red, float green, float blue, float alpha = 1);

		Color (void* pixel, const ColorSpace& cs);

		Color dup () const;

		Color& reset (float gray = 0,                     float alpha = 1);

		Color& reset (float red, float green, float blue, float alpha = 1);

		Color& reset8 (uchar gray = 0,                     uchar alpha = 255);

		Color& reset8 (uchar red, uchar green, uchar blue, uchar alpha = 255);

		Color& reset (const void* pixel, const ColorSpace& cs);

		void     get (      void* pixel, const ColorSpace& cs) const;

		operator bool () const;

		bool operator ! () const;

		static uchar float2uchar (float value) {return (uchar) Xot::clip(0.f, 255.f, value * 255);}

		static float uchar2float (uchar value) {return value / 255.f;}

	};// Color


	Color Color8 (uchar gray = 0,                     uchar alpha = 255);

	Color Color8 (uchar red, uchar green, uchar blue, uchar alpha = 255);


}// Rays


#endif//EOH
