#include "polygon.h"


#include <math.h>
#include <assert.h>
#include "rays/exception.h"
#include "rays/debug.h"
#include "polyline.h"


using namespace ClipperLib;


namespace Rays
{


	Polygon::Line::Line (const Polyline& polyline, bool hole)
	:	Super(polyline), hole(hole)
	{
	}

	Polygon::Line::operator bool () const
	{
		return loop() || !hole;
	}

	bool
	Polygon::Line::operator ! () const
	{
		return !operator bool();
	}


	struct Polygon::Data
	{

		LineList lines;

	};// Polygon::Data


	void
	Polygon_triangulate (const Polygon& polygon)
	{
		//polygon.self->polygons.get_trapezoids(polygon.self->trapezoids);
		//return polygon.self->trapezoids;
	}


	Polygon
	create_line(coord x1, coord y1, coord x2, coord y2)
	{
		const Point points[] = {
			Point(x1, y1),
			Point(x2, y2)
		};
		return create_line(points, 2, false);
	}

	Polygon
	create_line (const Point& p1, const Point& p2)
	{
		const Point points[] = {p1, p2};
		return create_line(points, 2, false);
	}

	Polygon
	create_line (const Point* points, size_t size, bool loop)
	{
		return Polygon(points, size, loop);
	}


	static Polygon
	create_unrounded_rect (coord x, coord y, coord width, coord height)
	{
		if (width == 0 || height == 0)
			return Polygon();

		const Point points[] = {
			Point(x,         y),
			Point(x,         y + height),
			Point(x + width, y + height),
			Point(x + width, y),
		};
		return Polygon(points, 4);
	}

	static void
	fix_rounds (
		coord* left_top, coord* right_top, coord* left_bottom, coord* right_bottom,
		coord width, coord height)
	{
		assert(left_top && right_top && left_bottom && right_bottom);

		if (width  < 0) width  = -width;
		if (height < 0) height = -height;

		coord* rounds[] = {left_top, right_top, right_bottom, left_bottom, left_top};
		coord sizes[]   = {width, height, width, height};

		for (size_t i = 0; i < 4; ++i)
		{
			const coord& size = sizes[i];

			coord* a = rounds[i];
			coord* b = rounds[i + 1];
			if (*a + *b <= size)
				continue;

			if (*a > *b)
				std::swap(a, b);

			if (*a * 2 > size)
				*a = *b = size / 2;
			else
				*b = size - *a;
		}
	}

	struct RoundedCorner
	{
		coord x, y, offset_sign_x, offset_sign_y, round;
	};

	static int
	setup_round (
		Point* point, coord x, coord y, const RoundedCorner& corner,
		coord sign_x, coord sign_y, float radian_offset, uint nsegment)
	{
		if (corner.round <= 0)
		{
			point->reset(x + corner.x, y + corner.y);
			return 1;
		}

		coord offset_x = corner.x + corner.round * corner.offset_sign_x * sign_x;
		coord offset_y = corner.y + corner.round * corner.offset_sign_y * sign_y;

		for (uint seg = 0; seg <= nsegment; ++seg)
		{
			float pos    = (float) seg / (float) nsegment;
			float radian = radian_offset + pos * (M_PI / 2);
			coord xx     = offset_x + cos(radian) * corner.round * sign_x;
			coord yy     = offset_y - sin(radian) * corner.round * sign_y;
			point->reset(x + xx, y + yy);
		}
		return nsegment + 1;
	}

	static Polygon
	create_rounded_rect (
		coord x, coord y, coord width, coord height,
		coord round_left_top,    coord round_right_top,
		coord round_left_bottom, coord round_right_bottom,
		uint nsegment)
	{
		assert(
			round_left_top    != 0 || round_right_top    != 0 ||
			round_left_bottom != 0 || round_right_bottom != 0);

		if (width == 0 || height == 0)
			return Polygon();

		if (nsegment < 1)
			nsegment = NSEGMENT_ROUND;

		fix_rounds(
			&round_left_top,    &round_right_top,
			&round_left_bottom, &round_right_bottom,
			width, height);

		coord sign_x = width  >= 0 ? +1 : -1;
		coord sign_y = height >= 0 ? +1 : -1;
		RoundedCorner corners[] =
		{
			{width,      0, -1, +1, round_right_top},
			{    0,      0, +1, +1, round_left_top},
			{    0, height, +1, -1, round_left_bottom},
			{width, height, -1, -1, round_right_bottom}
		};

		size_t npoints = 0;
		for (size_t i = 0; i < 4; ++i)
			npoints += corners[i].round > 0 ? nsegment + 1 : 1;

		std::unique_ptr<Point[]> points(new Point[npoints]);
		Point* point = points.get();

		for (size_t i = 0; i < 4; ++i)
		{
			point += setup_round(
				point, x, y, corners[i], sign_x, sign_y, (M_PI / 2) * i, nsegment);
		}

		return Polygon(points.get(), npoints);
	}

	Polygon
	create_rect (
		coord x, coord y, coord width, coord height,
		coord round, uint nsegment)
	{
		return create_rect(
			x, y, width, height, round, round, round, round, nsegment);
	}

	Polygon
	create_rect (
		coord x, coord y, coord width, coord height,
		coord round_left_top,    coord round_right_top,
		coord round_left_bottom, coord round_right_bottom,
		uint nsegment)
	{
		if (
			round_left_top    == 0 && round_right_top    == 0 &&
			round_left_bottom == 0 && round_right_bottom == 0)
		{
			return create_unrounded_rect(x, y, width, height);
		}
		else
		{
			return create_rounded_rect(
				x, y, width, height,
				round_left_top, round_right_top, round_left_bottom, round_right_bottom,
				nsegment);
		}
	}

	Polygon
	create_rect(const Bounds& bounds, coord round, uint nsegment)
	{
		return create_rect(bounds, round, round, round, round, nsegment);
	}

	Polygon
	create_rect (
		const Bounds& bounds,
		coord round_left_top,    coord round_right_top,
		coord round_left_bottom, coord round_right_bottom,
		uint nsegment)
	{
		if (
			round_left_top    == 0 && round_right_top    == 0 &&
			round_left_bottom == 0 && round_right_bottom == 0)
		{
			return create_unrounded_rect(
				bounds.x, bounds.y, bounds.width, bounds.height);
		}
		else
		{
			return create_rounded_rect(
				bounds.x, bounds.y, bounds.width, bounds.height,
				round_left_top, round_right_top, round_left_bottom, round_right_bottom,
				nsegment);
		}
	}

#if 0
	static void
	create_ellipse_without_hole (
		coord x, coord y, coord width, coord height,
		coord hole_width, coord hole_height,
		uint nsegment)
	{
		if (width == 0 || height == 0)
			return;

		if (nsegment < 3)
			nsegment = NSEGMENT_ELLIPSE;

		float from     = angle_from / 360.f;
		float to       = angle_to   / 360.f;
		bool hole      = hole_width != 0 && hole_height != 0;
		coord hole_x   = x + (width  - hole_width)  / 2;
		coord hole_y   = y + (height - hole_height) / 2;
		int npoint     = nsegment + 1;
		size_t npoints = hole ? npoint * 2 : npoint + 1;

		std::unique_ptr<Point[]> points(new Point[nvertices]);
		Point* point = points.get();

		if (!hole)
		{
			(point++)->reset(
				x + width  / 2,
				y + height / 2);
		}

		for (uint seg = 0; seg <= nsegment; ++seg)
		{
			float pos    = (float) seg / (float) nsegment;
			float radian = (from + (to - from) * pos) * PI_2;
			float cos_   = (cos(radian)  + 1) / 2.;
			float sin_   = (-sin(radian) + 1) / 2.;

			if (hole)
			{
				setup_ellipse_vertex(
					vertex++,
					hole_x,
					hole_y,
					hole_width  * cos_,
					hole_height * sin_);
			}

			setup_ellipse_vertex(
				vertex++,
				x,
				y,
				width  * cos_,
				height * sin_);
		}

		Color color;
		for (int type = COLOR_TYPE_BEGIN; type < COLOR_TYPE_END; ++type)
		{
			if (!self->get_color(&color, (ColorType) type))
				continue;

			self->draw_shape(
				modes[type], vertices.get(), nvertices, indices.get(), nvertices, color,
				get_default_shader_for_shape());
		}
	}

	Ellipse::Ellipse (
		coord x, coord y, coord width, coord height,
		float angle_from, float angle_to, const Point& hole_size, uint nsegment)
	{
		draw_ellipse(
			this, x, y, width, height,
			angle_from, angle_to, hole_size.x, hole_size.y, nsegment);
	}

	Ellipse::Ellipse (
		const Bounds& bounds,
		float angle_from, float angle_to, const Point& hole_size, uint nsegment)
	{
		ellipse(
			bounds.x, bounds.y, bounds.width, bounds.height,
			angle_from, angle_to, hole_size, nsegment);
	}

	Ellipse::Ellipse (
		const Point& center, coord radius,
		float angle_from, float angle_to, coord hole_radius, uint nsegment)
	{
		draw_ellipse(
			this, center.x - radius, center.y - radius, radius * 2, radius * 2,
			angle_from, angle_to, hole_radius * 2, hole_radius * 2, nsegment);
	}
#endif


	Polygon::Polygon ()
	{
	}

	Polygon::Polygon (const Point* points, size_t size, bool loop)
	{
		add(points, size, loop);
	}

	Polygon::Polygon (const Polyline& polyline)
	{
		add(polyline);
	}

	Polygon::~Polygon ()
	{
	}

	Polygon
	Polygon::dup () const
	{
		Polygon p;
		for (const auto& line : self->lines)
			p.add(line.dup(), line.hole);
		return p;
	}

	void
	Polygon::add (const Point* points, size_t size, bool loop, bool hole)
	{
		add(Polyline(points, size, loop), hole);
	}

	void
	Polygon::add (const Polyline& polyline, bool hole)
	{
		self->lines.emplace_back(Line(polyline, hole));
	}

	void
	Polygon::clear ()
	{
		self->lines.clear();
	}

	Polygon::const_iterator
	Polygon::begin () const
	{
		return self->lines.begin();
	}

	Polygon::const_iterator
	Polygon::end () const
	{
		return self->lines.end();
	}

	size_t
	Polygon::size () const
	{
		return self->lines.size();
	}

	bool
	Polygon::empty () const
	{
		return size() <= 0;
	}

	const Polygon::Line&
	Polygon::operator [] (size_t index) const
	{
		return self->lines[index];
	}

	Polygon::operator bool () const
	{
		return true;
	}

	bool
	Polygon::operator ! () const
	{
		return !operator bool();
	}

	static void
	add_polygon_to_clipper (Clipper* clipper, const Polygon& polygon, PolyType polytype)
	{
		assert(clipper);

		Path path;
		for (const auto& line : polygon)
		{
			if (!line) continue;

			Polyline_get_path(&path, line, line.hole);
			if (path.empty()) continue;

			clipper->AddPath(path, polytype, line.loop());
		}
	}

	static void
	get_polygon (Polygon* polygon, const PolyNode* node)
	{
		assert(polygon);

		for (const PolyNode* p = node; p; p = p->GetNext())
		{
			Polyline polyline;
			Polyline_create(&polyline, p->Contour, !p->IsOpen(), p->IsHole());
			if (!polyline) continue;

			polygon->add(polyline, p->IsHole());
		}
	}

	static Polygon
	clip (const Polygon& clipped, const Polygon& clipper, ClipType cliptype)
	{
		Clipper c;
		add_polygon_to_clipper(&c, clipped, ptSubject);
		add_polygon_to_clipper(&c, clipper, ptClip);

		PolyTree tree;
		c.Execute(cliptype, tree);

		Polygon result;
		get_polygon(&result, tree.GetFirst());

		return result;
	}

	Polygon&
	Polygon::operator -= (const Polygon& rhs)
	{
		if (&rhs == this) return *this;

		*this = clip(*this, rhs, ctDifference);
		return *this;
	}

	Polygon&
	Polygon::operator &= (const Polygon& rhs)
	{
		if (&rhs == this) return *this;

		*this = clip(*this, rhs, ctIntersection);
		return *this;
	}

	Polygon&
	Polygon::operator |= (const Polygon& rhs)
	{
		if (&rhs == this) return *this;

		*this = clip(*this, rhs, ctUnion);
		return *this;
	}

	Polygon&
	Polygon::operator ^= (const Polygon& rhs)
	{
		if (&rhs == this) return *this;

		*this = clip(*this, rhs, ctXor);
		return *this;
	}

	Polygon
	operator - (const Polygon& lhs, const Polygon& rhs)
	{
		Polygon s = lhs.dup();
		s -= rhs;
		return s;
	}

	Polygon
	operator & (const Polygon& lhs, const Polygon& rhs)
	{
		Polygon s = lhs.dup();
		s &= rhs;
		return s;
	}

	Polygon
	operator | (const Polygon& lhs, const Polygon& rhs)
	{
		Polygon s = lhs.dup();
		s |= rhs;
		return s;
	}

	Polygon
	operator ^ (const Polygon& lhs, const Polygon& rhs)
	{
		Polygon s = lhs.dup();
		s ^= rhs;
		return s;
	}


}// Rays
