#pragma once

#include "SpehsEngine/Math/Sphere.h"


namespace se
{
	bool checkOverlap(const Sphere& sphere1, const Sphere& sphere2);
	/* Returns true if given sphere overlaps with any other sphere inside the spheres vector. */
	bool checkOverlap(const Sphere& sphere, const std::vector<Sphere>& spheres);

	/* Returns true if containingSphere covers containedSphere completely. */
	bool checkContains(const Sphere& containingSphere, const Sphere& containedSphere);
	bool checkContains(const Sphere& containingSphere, const glm::vec3& containedPoint);
}
