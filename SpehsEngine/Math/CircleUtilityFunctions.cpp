#include "stdafx.h"
#include "SpehsEngine/Math/CircleUtilityFunctions.h"

#include "glm/geometric.hpp"

namespace se
{
	bool checkCircleOverlap(const Circle& circle, const std::vector<Circle>& circles)
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

	Bounds2D getBounds(const Circle& circle)
	{
		return Bounds2D(circle.position, glm::vec2(circle.radius, circle.radius));
	}
}
