#include "polygon.h"


#include <math.h>
#include "rays/exception.h"
#include "rays/debug.h"
#include "painter.h"


using namespace boost::polygon::operators;


namespace Rays
{


	struct Polygon::Data
	{

		BoostPolygonSet polygons;

		mutable BoostPolygonList trapezoids;

	};// Polygon::Data


	const BoostPolygonList&
	Polygon_subdivide (const Polygon& polygon)
	{
		polygon.self->polygons.get_trapezoids(polygon.self->trapezoids);
		return polygon.self->trapezoids;
	}

	const BoostPolygonSet&
	Polygon_get_boost_polygons (const Polygon& polygon)
	{
		return polygon.self->polygons;
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
			Point(x + width, y)
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
		add_outline(points, size, loop);
	}

	Polygon::~Polygon ()
	{
	}

	Polygon
	Polygon::dup () const
	{
		Polygon p;
		p.self->polygons = self->polygons;
		return p;
	}

	static void
	make_polygon (
		BoostPolygon* polygon, const Point* points, size_t size, bool loop)
	{
		assert(polygon);
		doutln("a: %d, %d", size, loop ? 1 : 0);

		if (!points || size < 2 || (size == 2 && loop))
			argument_error(__FILE__, __LINE__);

		std::vector<BoostPoint> tmp;
		tmp.reserve(size + loop ? 1 : 0);
		for (size_t i = 0; i < size; ++i)
			tmp.emplace_back(BoostPoint(points[i].x, points[i].y));

		if (loop)
			tmp.emplace_back(tmp[0]);

		polygon->set(tmp.begin(), tmp.end());
	}

	void
	Polygon::add_outline (const Point* points, size_t size, bool loop)
	{
		BoostPolygon polygon;
		make_polygon(&polygon, points, size, loop);

		self->polygons.insert(polygon);
	}

	void
	Polygon::add_hole (const Point* points, size_t size)
	{
		BoostPolygon polygon;
		make_polygon(&polygon, points, size, true);

		self->polygons.insert(polygon, true);
	}

	void
	Polygon::clear ()
	{
		self->polygons.clear();
	}

	Polygon::operator bool () const
	{
		return !self->polygons.empty();
	}

	bool
	Polygon::operator ! () const
	{
		return !operator bool();
	}

	Polygon&
	Polygon::operator -= (const Polygon& rhs)
	{
		if (&rhs == this) return *this;

		if (*this && rhs) self->polygons -= rhs.self->polygons;
		return *this;
	}

	Polygon&
	Polygon::operator &= (const Polygon& rhs)
	{
		if (&rhs == this) return *this;

		if (*this && rhs) self->polygons &= rhs.self->polygons;
		return *this;
	}

	Polygon&
	Polygon::operator |= (const Polygon& rhs)
	{
		if (&rhs == this) return *this;

		if (*this && rhs) self->polygons |= rhs.self->polygons;
		return *this;
	}

	Polygon&
	Polygon::operator ^= (const Polygon& rhs)
	{
		if (&rhs == this) return *this;

		if (*this && rhs) self->polygons ^= rhs.self->polygons;
		return *this;
	}

	Polygon
	operator - (const Polygon& lhs, const Polygon& rhs)
	{
		if (!lhs || !rhs) return Polygon();

		Polygon s = lhs.dup();
		s -= rhs;
		return s;
	}

	Polygon
	operator & (const Polygon& lhs, const Polygon& rhs)
	{
		if (!lhs || !rhs) return Polygon();

		Polygon s = lhs.dup();
		s &= rhs;
		return s;
	}

	Polygon
	operator | (const Polygon& lhs, const Polygon& rhs)
	{
		if (!lhs || !rhs) return Polygon();

		Polygon s = lhs.dup();
		s |= rhs;
		return s;
	}

	Polygon
	operator ^ (const Polygon& lhs, const Polygon& rhs)
	{
		if (!lhs || !rhs) return Polygon();

		Polygon s = lhs.dup();
		s ^= rhs;
		return s;
	}


}// Rays


#if 0
namespace boost
{


	namespace polygon
	{


		template <>
		struct geometry_concept<Rays::Point>
		{
			typedef point_concept type;
		};

		template <>
		struct geometry_concept<Rays::Polygon>
		{
			typedef polygon_concept type;
		};

		template <>
		struct geometry_concept<Rays::PolygonSet>
		{
			typedef polygon_set_concept type;
		};


		template <>
		struct point_traits<Rays::Point>
		{
			typedef Rays::coord coordinate_type;

			static inline coordinate_type get (
				const Rays::Point& point, orientation_2d orient)
			{
				return orient == HORIZONTAL ? point.x : point.y;
			}
		};


		template <>
		struct point_mutable_traits<Rays::Point>
		{
			typedef Rays::coord coordinate_type;

			static inline void set (
				Rays::Point& point, orientation_2d orient, coordinate_type value)
			{
				if (orient == HORIZONTAL)
					point.x = value;
				else
					point.y = value;
			}

			static inline Rays::Point construct (coordinate_type x, coordinate_type y)
			{
				return Rays::Point(x, y);
			}
		};


		template <>
		struct polygon_traits<Rays::Polygon>
		{
			typedef Rays::coord                   coordinate_type;

			typedef Rays::Point                   point_type;

			typedef Rays::Polygon::const_iterator iterator_type;

			static inline iterator_type begin_points (const Rays::Polygon& polygon)
			{
				return polygon.begin();
			}

			static inline iterator_type   end_points (const Rays::Polygon& polygon)
			{
				return polygon.end();
			}

			static inline std::size_t size (const Rays::Polygon& polygon)
			{
				return polygon.size();
			}

			static inline winding_direction winding (const Rays::Polygon& polygon)
			{
				return unknown_winding;
			}
		};


		template <>
		struct polygon_mutable_traits<Rays::Polygon>
		{
			template <typename IT>
			static inline Rays::Polygon& set_points (
				Rays::Polygon& polygon, IT begin, IT end)
			{
				polygon.clear();
				polygon.insert(polygon.end(), begin, end);
				return polygon;
			}
		};


		template <>
		struct polygon_set_traits<Rays::PolygonSet>
		{
			typedef Rays::coord                      coordinate_type;

			typedef Rays::PolygonSet::const_iterator iterator_type;

			typedef Rays::PolygonSet                 operator_arg_type;

			static inline iterator_type begin (const Rays::PolygonSet& polyset)
			{
				return polyset.begin();
			}

			static inline iterator_type end (const Rays::PolygonSet& polyset)
			{
				return polyset.end();
			}

			static inline bool clean (const Rays::PolygonSet& polyset)
			{
				return false;
			}

			static inline bool sorted (const Rays::PolygonSet& polyset)
			{
				return false;
			}
		};


		template <>
		struct polygon_set_mutable_traits<Rays::PolygonSet>
		{
			template <typename IT>
			static inline void set (Rays::PolygonSet& polyset, IT begin, IT end)
			{
				polyset.clear();

				polygon_set_data<Rays::coord> tmp;
				tmp.insert(begin, end);
				tmp.get(polyset);
			}
		};


	}// polygon


}// boost
#endif
