#include "rays/point.h"


#include <math.h>
#include <xot/string.h>
#include "rays/exception.h"


namespace Rays
{


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
		return sqrt(x * x + y * y + z * z);
	}

	void
	Point::normalize ()
	{
		coord len = length();
		if (len == 0)
			invalid_state_error(__FILE__, __LINE__);

		*this /= len;
	}

	Point
	Point::normal () const
	{
		Point t = *this;
		t.normalize();
		return t;
	}

	String
	Point::inspect () const
	{
		return Xot::stringf("x=%f y=%f z=%f", x, y, z);
	}

	Point
	Point::operator - () const
	{
		return Point(-x, -y, -z);
	}

	Point&
	Point::operator += (const Point& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}

	Point&
	Point::operator -= (const Point& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		return *this;
	}

	Point&
	Point::operator *= (const Point& rhs)
	{
		x *= rhs.x;
		y *= rhs.y;
		z *= rhs.z;
		return *this;
	}

	Point&
	Point::operator /= (const Point& rhs)
	{
		x /= rhs.x;
		y /= rhs.y;
		z /= rhs.z;
		return *this;
	}

	Point&
	Point::operator /= (coord rhs)
	{
		x /= rhs;
		y /= rhs;
		z /= rhs;
		return *this;
	}

	bool
	operator == (const Point& lhs, const Point& rhs)
	{
		return
			lhs.x == rhs.x &&
			lhs.y == rhs.y &&
			lhs.z == rhs.z;
	}

	bool
	operator != (const Point& lhs, const Point& rhs)
	{
		return !operator==(lhs, rhs);
	}

	Point
	operator + (const Point& lhs, const Point& rhs)
	{
		Point t = lhs;
		t += rhs;
		return t;
	}

	Point
	operator - (const Point& lhs, const Point& rhs)
	{
		Point t = lhs;
		t -= rhs;
		return t;
	}

	Point
	operator * (const Point& lhs, const Point& rhs)
	{
		Point t = lhs;
		t *= rhs;
		return t;
	}

	Point
	operator / (const Point& lhs, const Point& rhs)
	{
		Point t = lhs;
		t /= rhs;
		return t;
	}

	Point
	operator / (const Point& lhs, coord rhs)
	{
		Point t = lhs;
		t /= rhs;
		return t;
	}


}// Rays
