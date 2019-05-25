#pragma once
#include "glm/vec2.hpp"
#include "SpehsEngine/Math/LineSegment2DOrientation.h"
#include "SpehsEngine/Math/Bounds2D.h"

namespace se
{
	struct LineSegment2D;
	struct Bounds2D;

	bool checkOverlap(const LineSegment2D& lineSegment1, const LineSegment2D& lineSegment2);
	bool checkOverlap(const LineSegment2D& lineSegment, const Bounds2D& bounds2D);

	LineSegment2DOrientation getOrientation(const glm::vec2& begin, const glm::vec2& middle, const glm::vec2& end);

}
