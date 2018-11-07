#pragma once
#include <vector>
#include "SpehsEngine/Math/Circle.h"
#include "SpehsEngine/Math/Bounds2D.h"

namespace se
{
	bool checkOverlap(const Circle& circle1, const Circle& circle2);
	/* Returns true if given circle overlaps with any other circle inside the circles vector. */
	bool checkOverlap(const Circle& circle, const std::vector<Circle>& circles);

	/* Returns true if containingCircle covers containedCircle completely. */
	bool checkContains(const Circle& containingCircle, const Circle& containedCircle);

	Bounds2D getBounds(const Circle& circle);
}
