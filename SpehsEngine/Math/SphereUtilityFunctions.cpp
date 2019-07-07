#include "stdafx.h"
#include "SpehsEngine/Math/SphereUtilityFunctions.h"

#include "glm/geometric.hpp"

namespace se
{
	bool checkOverlap(const Sphere& sphere1, const Sphere& sphere2)
	{
		return glm::length(sphere1.position - sphere2.position) < sphere2.radius + sphere1.radius;
	}

	bool checkOverlap(const Sphere& sphere, const std::vector<Sphere>& spheres)
	{
		for (size_t m = 0; m < spheres.size(); m++)
		{
			if (checkOverlap(sphere, spheres[m]))
			{
				return true;
			}
		}
		return false;
	}

	bool checkContains(const Sphere& containingSphere, const Sphere& containedSphere)
	{
		if (containingSphere.radius < containedSphere.radius)
			return false;
		return (glm::length(containedSphere.position - containingSphere.position) + containedSphere.radius) <= containingSphere.radius;
	}

	bool checkContains(const Sphere& containingSphere, const glm::vec3& containedPoint)
	{
		return glm::length(containedPoint - containingSphere.position) <= containingSphere.radius;
	}
}
