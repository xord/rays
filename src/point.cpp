#include "rays/point.h"


#include <math.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#include <xot/string.h>
#include "rays/exception.h"


namespace Rays
{


	typedef glm::tvec2<coord> Vec2;

	typedef glm::tvec3<coord> Vec3;


	static inline       Vec3&  to_glm(      Point& val) {return *(      Vec3*)  &val;}

	static inline const Vec3&  to_glm(const Point& val) {return *(const Vec3*)  &val;}

	static inline       Point& to_rays(      Vec3& val) {return *(      Point*) &val;}

	static inline const Point& to_rays(const Vec3& val) {return *(const Point*) &val;}


	Coord2&
	Coord2::reset (coord value)
	{
		return reset(value, value);
	}

	Coord2&
	Coord2::reset (coord x, coord y)
	{
		this->x = x;
		this->y = y;
		return *this;
	}

	coord&
	Coord2::operator [] (size_t index)
	{
		if (index >= 2)
			argument_error(__FILE__, __LINE__);

		return array[index];
	}

	const coord&
	Coord2::operator [] (size_t index) const
	{
		return const_cast<Coord2*>(this)->operator[](index);
	}


	Coord3&
	Coord3::reset (coord value)
	{
		return reset(value, value, 0);
	}

	Coord3&
	Coord3::reset (coord x, coord y, coord z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		return *this;
	}

	coord&
	Coord3::operator [] (size_t index)
	{
		if (index >= 3)
			argument_error(__FILE__, __LINE__);

		return array[index];
	}

	const coord&
	Coord3::operator [] (size_t index) const
	{
		return const_cast<Coord3*>(this)->operator[](index);
	}


	Point::Point (coord value)
	{
		reset(value);
	}

	Point::Point (coord x, coord y, coord z)
	{
		reset(x, y, z);
	}

	Point
	Point::dup () const
	{
		return *this;
	}

	Point&
	Point::move_to (coord x, coord y, coord z)
	{
		reset(x, y, z);
		return *this;
	}

	Point&
	Point::move_to (const Point& point)
	{
		reset(point.x, point.y, point.z);
		return *this;
	}

	Point&
	Point::move_by (coord x, coord y, coord z)
	{
		reset(this->x + x, this->y + y, this->z + z);
		return *this;
	}

	Point&
	Point::move_by (const Point& point)
	{
		reset(this->x + point.x, this->y + point.y, this->z + point.z);
		return *this;
	}

	coord
	Point::length () const
	{
		return glm::length(to_glm(*this));
	}

	Point
	Point::normal () const
	{
		if (x == 0 && y == 0 && z == 0)
			invalid_state_error(__FILE__, __LINE__);

		return to_rays(glm::normalize(to_glm(*this)));
	}

	void
	Point::normalize ()
	{
		*this = normal();
	}

	String
	Point::inspect () const
	{
		return Xot::stringf("x=%f y=%f z=%f", x, y, z);
	}

	Point
	Point::operator - () const
	{
		return to_rays(-to_glm(*this));
	}

	Point&
	Point::operator += (coord rhs)
	{
		to_glm(*this) += rhs;
		return *this;
	}

	Point&
	Point::operator += (const Point& rhs)
	{
		to_glm(*this) += to_glm(rhs);
		return *this;
	}

	Point&
	Point::operator -= (coord rhs)
	{
		to_glm(*this) -= rhs;
		return *this;
	}

	Point&
	Point::operator -= (const Point& rhs)
	{
		to_glm(*this) -= to_glm(rhs);
		return *this;
	}

	Point&
	Point::operator *= (coord rhs)
	{
		to_glm(*this) *= rhs;
		return *this;
	}

	Point&
	Point::operator *= (const Point& rhs)
	{
		to_glm(*this) *= to_glm(rhs);
		return *this;
	}

	Point&
	Point::operator /= (coord rhs)
	{
		if (rhs == 0)
			argument_error(__FILE__, __LINE__);

		to_glm(*this) /= rhs;
		return *this;
	}

	Point&
	Point::operator /= (const Point& rhs)
	{
		if (rhs.x == 0 || rhs.y == 0 || rhs.z == 0)
			argument_error(__FILE__, __LINE__);

		to_glm(*this) /= to_glm(rhs);
		return *this;
	}

	bool
	operator == (const Point& lhs, const Point& rhs)
	{
		return to_glm(lhs) == to_glm(rhs);
	}

	bool
	operator != (const Point& lhs, const Point& rhs)
	{
		return to_glm(lhs) != to_glm(rhs);
	}

	Point
	operator + (coord lhs, const Point& rhs)
	{
		return to_rays(lhs + to_glm(rhs));
	}

	Point
	operator + (const Point& lhs, coord rhs)
	{
		return to_rays(to_glm(lhs) + rhs);
	}

	Point
	operator + (const Point& lhs, const Point& rhs)
	{
		return to_rays(to_glm(lhs) + to_glm(rhs));
	}

	Point
	operator - (coord lhs, const Point& rhs)
	{
		return to_rays(lhs - to_glm(rhs));
	}

	Point
	operator - (const Point& lhs, coord rhs)
	{
		return to_rays(to_glm(lhs) - rhs);
	}

	Point
	operator - (const Point& lhs, const Point& rhs)
	{
		return to_rays(to_glm(lhs) - to_glm(rhs));
	}

	Point
	operator * (coord lhs, const Point& rhs)
	{
		return to_rays(lhs * to_glm(rhs));
	}

	Point
	operator * (const Point& lhs, coord rhs)
	{
		return to_rays(to_glm(lhs) * rhs);
	}

	Point
	operator * (const Point& lhs, const Point& rhs)
	{
		return to_rays(to_glm(lhs) * to_glm(rhs));
	}

	Point
	operator / (coord lhs, const Point& rhs)
	{
		if (rhs.x == 0 || rhs.y == 0 || rhs.z == 0)
			argument_error(__FILE__, __LINE__);

		return to_rays(lhs / to_glm(rhs));
	}

	Point
	operator / (const Point& lhs, coord rhs)
	{
		if (rhs == 0)
			argument_error(__FILE__, __LINE__);

		return to_rays(to_glm(lhs) / rhs);
	}

	Point
	operator / (const Point& lhs, const Point& rhs)
	{
		if (rhs.x == 0 || rhs.y == 0 || rhs.z == 0)
			argument_error(__FILE__, __LINE__);

		return to_rays(to_glm(lhs) / to_glm(rhs));
	}


}// Rays
