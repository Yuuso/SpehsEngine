#pragma once

#include "glm/vec3.hpp"


namespace se
{
	struct Sphere
	{
		Sphere() = default;
		Sphere(const glm::vec3& _position, const float _radius) : position(_position), radius(_radius) {}
		glm::vec3 position;
		float radius = 0.0f;
	};
}
