// -*- c++ -*-
#pragma once
#ifndef __RAYS_EXT_DEFS_H__
#define __RAYS_EXT_DEFS_H__


#include <vector>
#include <rucy.h>
#include <rays/defs.h>
#include <rays/point.h>


using namespace Rucy;

using Rays::coord;


void get_line_args (
	std::vector<Rays::Point>* points, bool* loop_,
	Value args, Value loop);

void get_rect_args (
	coord* x,  coord* y,  coord* w,  coord* h,
	coord* lt, coord* rt, coord* lb, coord* rb, uint* nseg,
	Value args, Value round,
	Value lefttop, Value righttop, Value leftbottom, Value rightbottom,
	Value nsegment);

void get_ellipse_args (
	coord* x, coord* y, coord* w, coord* h,
	Rays::Point* hole_size, float* from, float* to, uint* nseg,
	Value args, Value center, Value radius,
	Value hole, Value angel_from, Value angle_to,
	Value nsegment);


#endif//EOH
