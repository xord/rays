// -*- c++ -*-
#pragma once
#ifndef __RAYS_PAINTER_H__
#define __RAYS_PAINTER_H__


#include <xot/pimpl.h>
#include <rays/defs.h>
#include <rays/opengl.h>


namespace Rays
{


	struct Coord2;

	struct Coord3;

	struct Point;

	struct Bounds;

	struct Color;

	struct Matrix;

	class Font;

	class Image;

	class Texture;

	class Shader;


	class Painter
	{

		public:

			enum
			{

				ELLIPSE_NSEGMENT = 32

			};

			Painter ();

			~Painter ();

			void canvas (coord x, coord y, coord width, coord height);

			void canvas (coord x, coord y, coord z, coord width, coord height, coord depth);

			void canvas (const Bounds& bounds);

			void bind (const Texture& texture);

			void unbind ();

			const Bounds& bounds () const;

			//
			// high level drawing methods
			//
			void begin ();

			void end ();

			void clear ();

			void line (coord x1, coord y1, coord x2, coord y2);

			void line (const Point& p1, const Point& p2);

			void lines   (const Coord2* points, size_t size);

			void lines   (const Coord3* points, size_t size);

			void polygon (const Coord2* points, size_t size);

			void polygon (const Coord3* points, size_t size);

			void rect (
				coord x, coord y, coord width, coord height,
				coord round = 0);

			void rect (
				coord x, coord y, coord width, coord height,
				coord round_width, coord round_height);

			void rect (
				const Bounds& bounds,
				coord round = 0);

			void rect (
				const Bounds& bounds,
				coord round_width, coord round_height);

			void ellipse (
				coord x, coord y, coord width, coord height = 0,
				coord radius_min = 0, uint nsegment = 0);

			void ellipse (
				const Bounds& bounds,
				coord radius_min = 0, uint nsegment = 0);

			void ellipse (
				const Point& center, coord radius,
				coord radius_min = 0, uint nsegment = 0);

			void arc (
				coord x, coord y, coord width, coord height = 0,
				float angle_from = 0, float angle_to = 360,
				coord radius_min = 0, uint nsegment = 0);

			void arc (
				const Bounds& bounds,
				float angle_from = 0, float angle_to = 360,
				coord radius_min = 0, uint nsegment = 0);

			void arc (
				const Point& center, coord radius,
				float angle_from = 0, float angle_to = 360,
				coord radius_min = 0, uint nsegment = 0);

			void image (
				const Image& image, coord x = 0, coord y = 0);

			void image (
				const Image& image, const Point& position);

			void image (
				const Image& image, coord x, coord y, coord width, coord height);

			void image (
				const Image& image, const Bounds& bounds);

			void image (
				const Image& image,
				coord  src_x, coord  src_y, coord src_width, coord src_height,
				coord dest_x, coord dest_y);

			void image (
				const Image& image,
				const Bounds& src_bounds, const Point& dest_position);

			void image (
				const Image& image,
				coord  src_x, coord  src_y, coord  src_width, coord  src_height,
				coord dest_x, coord dest_y, coord dest_width, coord dest_height);

			void image (
				const Image& image,
				const Bounds& src_bounds, const Bounds& dest_bounds);

			void text (
				const char* str, coord x = 0, coord y = 0,
				const Font* font = NULL);

			void text (
				const char* str, const Point& position,
				const Font* font = NULL);

			void text (
				const char* str, coord x, coord y, coord width, coord height,
				const Font* font = NULL);

			void text (
				const char* str, const Bounds& bounds,
				const Font* font = NULL);

			//
			// attributes
			//
			void     set_background (float red, float green, float blue, float alpha = 1, bool clear = true);

			void     set_background (const Color& color, bool clear = true);

			void      no_background (bool clear = true);

			const Color& background () const;

			void     set_fill (float red, float green, float blue, float alpha = 1);

			void     set_fill (const Color& color);

			void      no_fill ();

			const Color& fill () const;

			void     set_stroke (float red, float green, float blue, float alpha = 1);

			void     set_stroke (const Color& color);

			void      no_stroke ();

			const Color& stroke () const;

			void      set_clip (coord x, coord y, coord width, coord height);

			void      set_clip (const Bounds& bounds);

			void       no_clip ();

			const Bounds& clip () const;

			void    set_font (const char* name, coord size = 0);

			void    set_font (const Font& font);

			const Font& font () const;

			void push_attr ();

			void  pop_attr ();

			//
			// shader manipulation methods
			//
			void attach (const Shader& shader);

			void detach (const Shader& shader);

			void set_uniform (const char* name, int arg1);

			void set_uniform (const char* name, int arg1, int arg2);

			void set_uniform (const char* name, int arg1, int arg2, int arg3);

			void set_uniform (const char* name, int arg1, int arg2, int arg3, int arg4);

			void set_uniform (const char* name, const int* args, size_t size);

			void set_uniform (const char* name, float arg1);

			void set_uniform (const char* name, float arg1, float arg2);

			void set_uniform (const char* name, float arg1, float arg2, float arg3);

			void set_uniform (const char* name, float arg1, float arg2, float arg3, float arg4);

			void set_uniform (const char* name, const float* args, size_t size);

			void push_shader ();

			void  pop_shader ();

			//
			// transformation methods
			//
			void translate (coord x, coord y, coord z = 0);

			void translate (const Point& value);

			void scale (coord x, coord y, coord z = 1);

			void scale (const Point& value);

			void rotate (float angle, coord x = 0, coord y = 0, coord z = 1);

			void rotate (float angle, const Point& axis);

			void set_matrix (float value = 1);

			void set_matrix (
				float a1, float a2, float a3, float a4,
				float b1, float b2, float b3, float b4,
				float c1, float c2, float c3, float c4,
				float d1, float d2, float d3, float d4);

			void set_matrix (const float* elements);

			void set_matrix (const Matrix& matrix);

			const Matrix& matrix () const;

			void push_matrix ();

			void pop_matrix ();


			operator bool () const;

			bool operator ! () const;

			struct Data;

			Xot::PImpl<Data, true> self;

	};// Painter


}// Rays


#endif//EOH
