#include "polygon.h"


#include <math.h>
#include <assert.h>
#include <poly2tri.h>
#include "xot/util.h"
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

		void append (const Polyline& polyline, bool hole = false)
		{
			if (polyline.empty())
				return;

			lines.emplace_back(Line(polyline, hole));
		}

		void copy_to (Data* data)
		{
			data->lines = lines;
		}

	};// Polygon::Data


	Polygon
	create_line(coord x1, coord y1, coord x2, coord y2)
	{
		const Point points[] = {
			Point(x1, y1),
			Point(x2, y2)
		};
		return create_line(points, 2);
	}

	Polygon
	create_line (const Point& p1, const Point& p2)
	{
		const Point points[] = {p1, p2};
		return create_line(points, 2);
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

	static inline Point
	make_ellipse_points (
		coord x, coord y, coord width, coord height,
		float radian_from, float radian_to,
		uint segment_max, uint segment_index)
	{
		float pos    = (float) segment_index / (float) segment_max;
		float radian = radian_from + (radian_to - radian_from) * pos;
		float cos_   = (cos(radian)  + 1) / 2.;
		float sin_   = (-sin(radian) + 1) / 2.;

		return Point(
			x + width  * cos_,
			y + height * sin_);
	}

	static Polygon
	create_ellipse (
		coord x, coord y, coord width, coord height,
		const Point& hole_size, uint nsegment)
	{
		assert(width > 0 && height > 0);

		if (nsegment < 3)
			nsegment = NSEGMENT_ELLIPSE;

		bool has_hole = hole_size != 0;
		uint npoint   = nsegment + 1;

		std::vector<Point> points;
		points.reserve(npoint);

		for (uint seg = 0; seg < nsegment; ++seg)
		{
			points.emplace_back(
				make_ellipse_points(x, y, width, height, 0, M_PI, nsegment, seg));
		}

		Polygon polygon;
		polygon.self->append(Polyline(&points[0], points.size()));

		if (has_hole)
		{
			coord hole_x = x + (width  - hole_size.x) / 2;
			coord hole_y = y + (height - hole_size.y) / 2;

			points.clear();
			for (uint seg = 0; seg < nsegment; ++seg)
			{
				points.emplace_back(make_ellipse_points(
					hole_x, hole_y, hole_size.x, hole_size.y, 0, M_PI, nsegment, seg));
			}
			polygon.self->append(Polyline(&points[0], points.size()), true);
		}

		return polygon;
	}

	static Polygon
	create_arc (
		coord x, coord y, coord width, coord height,
		float angle_from, float angle_to, const Point& hole_size, uint nsegment)
	{
		assert(width > 0 && height > 0 && angle_from != angle_to);

		if (nsegment < 2)
			nsegment = NSEGMENT_ELLIPSE;

		bool has_hole     = hole_size != 0;
		float radian_from = Xot::deg2rad(angle_from);
		float radian_to   = Xot::deg2rad(angle_to);
		uint npoint       = nsegment + 1;

		std::vector<Point> points;
		points.reserve(has_hole ? npoint * 2 : npoint + 1);

		for (uint seg = 0; seg <= nsegment; ++seg)
		{
			points.emplace_back(make_ellipse_points(
				x, y, width, height, radian_from, radian_to, nsegment, seg));
		}

		if (has_hole)
		{
			coord hole_x = x + (width  - hole_size.x) / 2;
			coord hole_y = y + (height - hole_size.y) / 2;

			for (uint seg = nsegment; seg >= 0; --seg)
			{
				points.emplace_back(make_ellipse_points(
					hole_x, hole_y, hole_size.x, hole_size.y,
					radian_from, radian_to, nsegment, seg));
			}
		}
		else
		{
			points.emplace_back(Point(
				x + width  / 2,
				y + height / 2));
		}

		return Polygon(&points[0], points.size());
	}

	Polygon
	create_ellipse (
		coord x, coord y, coord width, coord height,
		float angle_from, float angle_to, const Point& hole_size, uint nsegment)
	{
		if (width == 0 || height == 0 || angle_from == angle_to)
			return Polygon();

		if (angle_from == 0 && angle_to == 360)
			return create_ellipse(x, y, width, height, hole_size, nsegment);
		else
		{
			return create_arc(
				x, y, width, height, angle_from, angle_to, hole_size, nsegment);
		}
	}

	Polygon
	create_ellipse (
		const Bounds& bounds,
		float angle_from, float angle_to, const Point& hole_size, uint nsegment)
	{
		return create_ellipse(
			bounds.x, bounds.y, bounds.width, bounds.height,
			angle_from, angle_to, hole_size, nsegment);
	}

	Polygon
	create_ellipse (
		const Point& center, const Point& radius,
		float angle_from, float angle_to, coord hole_radius, uint nsegment)
	{
		return create_ellipse(
			center.x - radius.x, center.y - radius.y,
			center.x + radius.x, center.y + radius.y,
			angle_from, angle_to,
			hole_radius * 2, nsegment);
	}


	static inline p2t::Point
	to_poly2tri (const Point& point)
	{
		return p2t::Point(point.x, point.y);
	}

	static inline Point
	from_poly2tri (const p2t::Point& point)
	{
		return Point(point.x, point.y);
	}

	static size_t
	count_points (const Polygon& polygon)
	{
		size_t count = 0;
		for (const auto& line : polygon)
			count += line.size();
		return count;
	}

	static void
	triangulate (std::vector<Point>* triangles, p2t::CDT* cdt)
	{
		assert(triangles && cdt);

		cdt->Triangulate();

		for (auto* triangle : cdt->GetTriangles())
		{
			for (int i = 0; i < 3; ++i)
				triangles->emplace_back(from_poly2tri(*triangle->GetPoint(i)));
		}
	}

	void
	Polygon_triangulate (std::vector<Point>* triangles, const Polygon& polygon)
	{
		assert(triangles);

		triangles->clear();

		size_t npoint = count_points(polygon);
		if (npoint <= 0) return;

		std::unique_ptr<p2t::CDT> cdt;
		std::vector<p2t::Point> points;
		std::vector<p2t::Point*> pointers;

		points.reserve(npoint);
		for (const auto& line : polygon)
		{
			pointers.clear();
			pointers.reserve(line.size());
			for (const auto& point : line)
			{
				points.emplace_back(to_poly2tri(point));
				pointers.emplace_back(&points.back());
			}

			if (!line.hole)
			{
				if (cdt) triangulate(triangles, cdt.get());
				cdt.reset(new p2t::CDT(pointers));
			}
			else if (cdt)
				cdt->AddHole(pointers);
		}

		if (cdt) triangulate(triangles, cdt.get());
	}


	Polygon::Polygon ()
	{
	}

	Polygon::Polygon (const Point* points, size_t size, bool loop)
	{
		if (size <= 0) return;

		self->append(Polyline(points, size, loop));
	}

	Polygon::Polygon (const Polyline& polyline)
	{
		self->append(polyline);
	}

	Polygon::~Polygon ()
	{
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

#if 0
	static String
	path2str (const ClipperLib::Path& path)
	{
		String s;
		for (const auto& point : path)
		{
			if (!s.empty()) s += ", ";
			s += Xot::stringf("[%d,%d]", point.X / 1000, point.Y / 1000);
		}
		return s;
	}

	static void
	dout_node (const ClipperLib::PolyNode& node)
	{
		doutln(
			"path(open: %d, hole: %d, Contour: %s)",
			(int) node.IsOpen(),
			(int) node.IsHole(),
			path2str(node.Contour).c_str());
	}

	static void
	dout_tree (const PolyNode& node, int level = 0)
	{
		for (int i = 0; i < level; ++i) dout("  ");
		dout_node(node);

		for (const auto* child : node.Childs)
			dout_tree(*child, level + 1);
	}
#endif

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

	static bool
	append_outline (Polygon* polygon, const PolyNode& node)
	{
		assert(polygon);

		if (node.Contour.empty() || node.IsHole())
			return false;

		Polyline polyline;
		Polyline_create(&polyline, node.Contour, !node.IsOpen(), false);
		if (!polyline)
			return false;

		polygon->self->append(polyline, false);
		return true;
	}

	static void
	append_hole (Polygon* polygon, const PolyNode& node)
	{
		assert(polygon);

		for (const auto* child : node.Childs)
		{
			if (!child->IsHole())
				return;

			Polyline polyline;
			Polyline_create(&polyline, child->Contour, !child->IsOpen(), true);
			if (!polyline)
				continue;

			polygon->self->append(polyline, true);
		}
	}

	static void
	get_polygon (Polygon* polygon, const PolyNode& node)
	{
		assert(polygon);

		if (append_outline(polygon, node))
			append_hole(polygon, node);

		for (const auto* child : node.Childs)
			get_polygon(polygon, *child);
	}

	static Polygon
	clip (const Polygon& clipped, const Polygon& clipper, ClipType cliptype)
	{
		Clipper c;
		c.StrictlySimple(true);

		add_polygon_to_clipper(&c, clipped, ptSubject);
		add_polygon_to_clipper(&c, clipper, ptClip);

		PolyTree tree;
		c.Execute(cliptype, tree);
		assert(tree.Contour.empty());

		Polygon result;
		get_polygon(&result, tree);

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

	static Polygon
	dup (const Polygon& obj)
	{
		Polygon p;
		obj.self->copy_to(p.self.get());
		return p;
	}

	Polygon
	operator - (const Polygon& lhs, const Polygon& rhs)
	{
		return dup(lhs) -= rhs;
	}

	Polygon
	operator & (const Polygon& lhs, const Polygon& rhs)
	{
		return dup(lhs) &= rhs;
	}

	Polygon
	operator | (const Polygon& lhs, const Polygon& rhs)
	{
		return dup(lhs) |= rhs;
	}

	Polygon
	operator ^ (const Polygon& lhs, const Polygon& rhs)
	{
		return dup(lhs) ^= rhs;
	}


}// Rays
