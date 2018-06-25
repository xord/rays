#include "polygon.h"


#include <math.h>
#include <assert.h>
#include <utility>
#include <poly2tri.h>
#include "xot/util.h"
#include "rays/exception.h"
#include "rays/debug.h"
#include "polyline.h"
#include "painter.h"


using namespace ClipperLib;


namespace Rays
{


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

	static uint
	get_nsegment (
		uint nsegment, uint nsegment_min, float angle_from, float angle_to)
	{
		float angle = angle_to - angle_from;
		assert(0 <= angle && angle <= 360);

		if (nsegment <= 0)
			nsegment = 32;
		else if (nsegment < nsegment_min)
			nsegment = nsegment_min;

		nsegment *= angle / 360;
		return nsegment > 0 ? nsegment : 1;
	}


	struct Polygon::Data
	{

		LineList lines;

		Data ()
		{
		}

		virtual ~Data ()
		{
		}

		void append (const Polyline& polyline, bool hole = false)
		{
			if (polyline.empty())
				return;

			lines.emplace_back(Line(polyline, hole));
		}

		bool triangulate (TrianglePointList* triangles) const
		{
			assert(triangles);

			triangles->clear();

			if (!can_triangulate())
				return false;

			size_t npoint = count_points();
			if (npoint <= 0)
				return true;

			std::unique_ptr<p2t::CDT> cdt;
			std::vector<p2t::Point> points;
			std::vector<p2t::Point*> pointers;

			points.reserve(npoint);
			for (const auto& line : lines)
			{
				pointers.clear();
				pointers.reserve(line.size());
				for (const auto& point : line)
				{
					points.emplace_back(to_poly2tri(point));
					pointers.emplace_back(&points.back());
				}

				if (!line.hole())
				{
					if (cdt) triangulate(triangles, cdt.get());
					cdt.reset(new p2t::CDT(pointers));
				}
				else if (cdt)
					cdt->AddHole(pointers);
			}

			if (cdt) triangulate(triangles, cdt.get());

			return true;
		}

		virtual void fill (Painter* painter, const Color& color) const = 0;

		private:

			bool can_triangulate () const
			{
				for (const auto& line : lines)
				{
					if (line.loop() && !line.hole() && !line.empty())
						return true;
				}
				return false;
			}

			size_t count_points () const
			{
				size_t count = 0;
				for (const auto& line : lines)
					count += line.size();
				return count;
			}

			void triangulate (TrianglePointList* triangles, p2t::CDT* cdt) const
			{
				assert(triangles && cdt);

				cdt->Triangulate();

				for (auto* triangle : cdt->GetTriangles())
				{
					for (int i = 0; i < 3; ++i)
						triangles->emplace_back(from_poly2tri(*triangle->GetPoint(i)));
				}
			}

	};// Polygon::Data


	struct PolygonData : public Polygon::Data
	{

		mutable TrianglePointList triangles;

		void fill (Painter* painter, const Color& color) const
		{
			if (triangles.empty())
			{
				if (!triangulate(&triangles))
					return;
			}

			Painter_fill_polygon(
				painter, GL_TRIANGLES, color, &triangles[0], triangles.size());
		}

	};// PolygonData


	struct RectData : public Polygon::Data
	{

		RectData (
			coord x, coord y, coord width, coord height,
			coord round_lefttop,    coord round_righttop,
			coord round_leftbottom, coord round_rightbottom,
			uint nsegment)
		{
			if (
				round_lefttop    == 0 && round_righttop    == 0 &&
				round_leftbottom == 0 && round_rightbottom == 0)
			{
				setup_rect(x, y, width, height, nsegment);
			}
			else
			{
				setup_round_rect(
					x, y, width, height,
					round_lefttop, round_righttop, round_leftbottom, round_rightbottom,
					nsegment);
			}
		}

		void fill (Painter* painter, const Color& color) const
		{
			if (lines.size() != 1)
				invalid_state_error(__FILE__, __LINE__);

			const auto& outline = lines[0];
			Painter_fill_polygon(
				painter, GL_TRIANGLE_FAN, color, &outline[0], outline.size());
		}

		private:

			struct RoundedCorner
			{
				coord x, y, offset_sign_x, offset_sign_y, round;
			};

			void setup_rect (
				coord x, coord y, coord width, coord height,
				uint nsegment)
			{
				const Point points[] = {
					Point(x,         y),
					Point(x,         y + height),
					Point(x + width, y + height),
					Point(x + width, y),
				};
				append(Polyline(points, 4));
			}

			void setup_round_rect (
				coord x, coord y, coord width, coord height,
				coord lefttop, coord righttop, coord leftbottom, coord rightbottom,
				uint nsegment)
			{
				assert(width != 0 && height != 0);
				assert(
					lefttop    != 0 || righttop    != 0 ||
					leftbottom != 0 || rightbottom != 0);

				nsegment = get_nsegment(nsegment, 1, 0, 90);

				fix_rounds(
					&lefttop, &righttop, &leftbottom, &rightbottom, width, height);

				coord sign_x            = width  >= 0 ? +1 : -1;
				coord sign_y            = height >= 0 ? +1 : -1;
				RoundedCorner corners[] =
				{
					{width,      0, -1, +1, righttop},
					{    0,      0, +1, +1, lefttop},
					{    0, height, +1, -1, leftbottom},
					{width, height, -1, -1, rightbottom}
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

				append(Polyline(points.get(), npoints));
			}

			void fix_rounds (
				coord* lefttop, coord* righttop, coord* leftbottom, coord* rightbottom,
				coord width, coord height)
			{
				assert(lefttop && righttop && leftbottom && rightbottom);

				if (width  < 0) width  = -width;
				if (height < 0) height = -height;

				coord* rounds[] = {lefttop, righttop, rightbottom, leftbottom, lefttop};
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

			int setup_round (
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

				for (uint seg = 0; seg <= nsegment; ++seg, ++point)
				{
					float pos    = (float) seg / (float) nsegment;
					float radian = radian_offset + pos * (M_PI / 2);
					point->reset(
						x + offset_x + cos(radian) * corner.round * sign_x,
						y + offset_y - sin(radian) * corner.round * sign_y);
				}
				return nsegment + 1;
			}

	};// RectData


	struct EllipseData : public PolygonData
	{

		typedef PolygonData Super;

		GLenum mode = 0;

		std::vector<uint> indices;

		EllipseData (
			coord x, coord y, coord width, coord height,
			const Point& hole_size,
			float angle_from, float angle_to,
			uint nsegment)
		{
			normalize_angle(&angle_from, &angle_to);

			if ((angle_to - angle_from) >= 360)
				setup_ellipse(x, y, width, height, hole_size, nsegment);
			else
			{
				setup_arc(
					x, y, width, height, hole_size, angle_from, angle_to, nsegment);
			}
		}

		void fill (Painter* painter, const Color& color) const
		{
			if (lines.size() <= 0)
				invalid_state_error(__FILE__, __LINE__);

			if (mode == 0)
			{
				Super::fill(painter, color);
				return;
			}

			if (lines.size() >= 2)
				invalid_state_error(__FILE__, __LINE__);

			const auto& outline = lines[0];
			Painter_fill_polygon(
				painter, mode, color,
				&outline[0], outline.size(), &indices[0], indices.size());
		}

		private:

			void normalize_angle (float* angle_from, float* angle_to)
			{
				assert(angle_from && angle_to);

				float& from = *angle_from;
				float& to   = *angle_to;

				if (from > to)
					std::swap(from, to);

				if (to - from > 360)
					to = from + 360;
			}

			void setup_ellipse (
				coord x, coord y, coord width, coord height,
				const Point& hole_size, uint nsegment)
			{
				assert(width != 0 && height != 0);

				nsegment = get_nsegment(nsegment, 3, 0, 360);

				bool has_hole     = hole_size != 0;
				float radian_from = Xot::deg2rad(0);
				float radian_to   = Xot::deg2rad(360);

				if (!has_hole) mode = GL_TRIANGLE_FAN;

				std::vector<Point> points;
				points.reserve(nsegment);

				for (uint seg = 0; seg < nsegment; ++seg)
				{
					points.emplace_back(make_ellipse_point(
						x, y, width, height, radian_from, radian_to, nsegment, seg));
				}
				append(Polyline(&points[0], points.size()));

				if (has_hole)
				{
					points.clear();

					coord hole_x = x + (width  - hole_size.x) / 2;
					coord hole_y = y + (height - hole_size.y) / 2;
					for (uint seg = 0; seg < nsegment; ++seg)
					{
						points.emplace_back(make_ellipse_point(
							hole_x, hole_y, hole_size.x, hole_size.y,
							radian_from, radian_to, nsegment, seg));
					}
					append(Polyline(&points[0], points.size()), true);
				}
			}

			void setup_arc (
				coord x, coord y, coord width, coord height,
				const Point& hole_size, float angle_from, float angle_to,
				uint nsegment)
			{
				assert(width != 0 && height != 0 && angle_from != angle_to);

				nsegment = get_nsegment(nsegment, 3, angle_from, angle_to);

				bool has_hole     = hole_size != 0;
				float radian_from = Xot::deg2rad(angle_from);
				float radian_to   = Xot::deg2rad(angle_to);
				uint npoint       = nsegment + 1;

				std::vector<Point> points;
				points.reserve(has_hole ? npoint * 2 : npoint + 1);

				if (!has_hole)
				{
					points.emplace_back(Point(x + width / 2, y + height / 2));
					mode = GL_TRIANGLE_FAN;
				}

				for (uint seg = 0; seg <= nsegment; ++seg)
				{
					points.emplace_back(make_ellipse_point(
						x, y, width, height, radian_from, radian_to, nsegment, seg));
				}

				if (has_hole)
				{
					coord hole_x = x + (width  - hole_size.x) / 2;
					coord hole_y = y + (height - hole_size.y) / 2;
					for (uint seg = 0; seg <= nsegment; ++seg)
					{
						points.emplace_back(make_ellipse_point(
							hole_x, hole_y, hole_size.x, hole_size.y,
							radian_from, radian_to, nsegment, nsegment - seg));
					}
				}

				append(Polyline(&points[0], points.size()));
			}

			Point make_ellipse_point (
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

	};// EllipseData


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
		coord round_lefttop,    coord round_righttop,
		coord round_leftbottom, coord round_rightbottom,
		uint nsegment)
	{
		if (width == 0 || height == 0)
			return Polygon();

		return Polygon(new RectData(
			x, y, width, height,
			round_lefttop, round_righttop, round_leftbottom, round_rightbottom,
			nsegment));
	}

	Polygon
	create_rect(const Bounds& bounds, coord round, uint nsegment)
	{
		return create_rect(
			bounds.x, bounds.y, bounds.width, bounds.height,
			round, round, round, round,
			nsegment);
	}

	Polygon
	create_rect (
		const Bounds& bounds,
		coord round_lefttop,    coord round_righttop,
		coord round_leftbottom, coord round_rightbottom,
		uint nsegment)
	{
		return create_rect(
			bounds.x, bounds.y, bounds.width, bounds.height,
			round_lefttop, round_righttop, round_leftbottom, round_rightbottom,
			nsegment);
	}

	Polygon
	create_ellipse (
		coord x, coord y, coord width, coord height,
		const Point& hole_size,
		float angle_from, float angle_to,
		uint nsegment)
	{
		if (width == 0 || height == 0 || angle_from == angle_to)
			return Polygon();

		return Polygon(new EllipseData(
			x, y, width, height, hole_size, angle_from, angle_to, nsegment));
	}

	Polygon
	create_ellipse (
		const Bounds& bounds,
		const Point& hole_size,
		float angle_from, float angle_to,
		uint nsegment)
	{
		return create_ellipse(
			bounds.x, bounds.y, bounds.width, bounds.height,
			hole_size, angle_from, angle_to, nsegment);
	}

	Polygon
	create_ellipse (
		const Point& center, const Point& radius,
		const Point& hole_radius,
		float angle_from, float angle_to,
		uint nsegment)
	{
		return create_ellipse(
			center.x - radius.x, center.y - radius.y,
			center.x + radius.x, center.y + radius.y,
			hole_radius * 2, angle_from, angle_to, nsegment);
	}

	void
	Polygon_fill (const Polygon& polygon, Painter* painter, const Color& color)
	{
		polygon.self->fill(painter, color);
	}

	bool
	Polygon_triangulate (TrianglePointList* triangles, const Polygon& polygon)
	{
		return polygon.self->triangulate(triangles);
	}


	Polygon::Polygon ()
	:	self(new PolygonData())
	{
	}

	Polygon::Polygon (const Point* points, size_t size, bool loop)
	:	self(new PolygonData())
	{
		if (!points || size <= 0) return;

		self->append(Polyline(points, size, loop));
	}

	Polygon::Polygon (const Polyline& polyline)
	:	self(new PolygonData())
	{
		self->append(polyline);
	}

	Polygon::Polygon (Data* data)
	:	self(data)
	{
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
	add_polygon_to_clipper (
		Clipper* clipper, const Polygon& polygon, PolyType polytype)
	{
		assert(clipper);

		Path path;
		for (const auto& line : polygon)
		{
			if (!line) continue;

			Polyline_get_path(&path, line, line.hole());
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
		p.self->lines = obj.self->lines;
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


	Polygon::Line::Line (const Polyline& polyline, bool hole)
	:	Super(polyline), hole_(hole)
	{
	}

	bool
	Polygon::Line::hole () const
	{
		return hole_;
	}

	Polygon::Line::operator bool () const
	{
		if (!Super::operator bool())
			return false;

		return loop() || !hole();
	}

	bool
	Polygon::Line::operator ! () const
	{
		return !operator bool();
	}


}// Rays
