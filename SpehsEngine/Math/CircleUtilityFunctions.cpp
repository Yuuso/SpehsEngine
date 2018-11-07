#include "stdafx.h"
#include "SpehsEngine/Math/CircleUtilityFunctions.h"

#include "glm/geometric.hpp"

namespace se
{
	bool checkOverlap(const Circle& circle1, const Circle& circle2)
	{
		return glm::length(circle1.position - circle2.position) <= circle2.radius - circle1.radius;
	}

	bool checkOverlap(const Circle& circle, const std::vector<Circle>& circles)
	{
		for (size_t m = 0; m < circles.size(); m++)
		{
			if (glm::length(circle.position - circles[m].position) <= circles[m].radius - circle.radius)
			{
				return true;
			}
		}
		return false;
	}

	bool checkContains(const Circle& containingCircle, const Circle& containedCircle)
	{
		if (containingCircle.radius < containedCircle.radius)
			return false;
		return (glm::length(containedCircle.position - containingCircle.position) + containedCircle.radius) <= containingCircle.radius;
	}

	Bounds2D getBounds(const Circle& circle)
	{
		return Bounds2D(circle.position, glm::vec2(circle.radius, circle.radius));
	}
}
