#pragma once
#include <vector>
#include "SpehsEngine/Math/Circle.h"

namespace se
{
	/*
	Returns true if given circle overlaps with any other circle inside the circles vector.
	*/
	bool checkCircleOverlap(const Circle& circle, const std::vector<Circle>& circles);
}
