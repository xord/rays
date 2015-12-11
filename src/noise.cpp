#include "rays/noise.h"


#include <glm/gtc/noise.hpp>
#include "point.h"


namespace Rays
{


	coord
	perlin (coord x)
	{
		return (coord) glm::perlin(glm::vec2(x, 0));
	}

	coord
	perlin (coord x, coord y)
	{
		return (coord) glm::perlin(glm::vec2(x, y));
	}

	coord
	perlin (coord x, coord y, coord z)
	{
		return (coord) glm::perlin(glm::vec3(x, y, z));
	}

	coord
	perlin (coord x, coord y, coord z, coord w)
	{
		return (coord) glm::perlin(glm::vec4(x, y, z, w));
	}

	coord
	perlin (const Point& point)
	{
		return (coord) glm::perlin(to_glm(point));
	}


	coord
	simplex (coord x)
	{
		return (coord) glm::simplex(glm::vec2(x, 0));
	}

	coord
	simplex (coord x, coord y)
	{
		return (coord) glm::simplex(glm::vec2(x, y));
	}

	coord
	simplex (coord x, coord y, coord z)
	{
		return (coord) glm::simplex(glm::vec3(x, y, z));
	}

	coord
	simplex (coord x, coord y, coord z, coord w)
	{
		return (coord) glm::simplex(glm::vec4(x, y, z, w));
	}

	coord
	simplex (const Point& point)
	{
		return (coord) glm::simplex(to_glm(point));
	}


}// Rays
