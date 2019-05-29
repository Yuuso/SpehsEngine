#pragma once
#include <optional>

namespace se
{
	std::optional<glm::vec2> calculateAimPrediction(const glm::vec2& origin, const glm::vec2& targetPosition, const glm::vec2& targetVelocity, const float projectileSpeed);
}
