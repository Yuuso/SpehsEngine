#pragma once
#include <vector>
#include "glm/vec2.hpp"

namespace se
{
	struct Circle
	{
		Circle() = default;
		Circle(const glm::vec2& _position, const float _radius) : position(_position), radius(_radius) {}
		glm::vec2 position;
		float radius = 0.0f;
	};
}
