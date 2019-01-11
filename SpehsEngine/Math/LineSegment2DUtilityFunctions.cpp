/*
	Source: https://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/
*/
#include "stdafx.h"
#include "SpehsEngine/Math/LineSegment2DUtilityFunctions.h"

#include <algorithm>
#include "SpehsEngine/Math/LineSegment2D.h"

namespace se
{
	// Given three colinear points p, q, r, the function checks if point q lies on line segment 'pr' 
	bool onSegment(const glm::vec2& p, const glm::vec2& q, const glm::vec2& r)
	{
		if (q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) && q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	LineSegment2DOrientation getOrientation(const glm::vec2& begin, const glm::vec2& middle, const glm::vec2& end)
	{
		const float val = (middle.y - begin.y) * (end.x - middle.x) - (middle.x - begin.x) * (end.y - middle.y);
		if (val == 0.0f)
		{
			return LineSegment2DOrientation::colinear;
		}
		else if (val > 0.0f)
		{
			return LineSegment2DOrientation::clockwise;
		}
		else
		{
			return LineSegment2DOrientation::counterclockwise;
		}
	}

	bool checkOverlap(const LineSegment2D& lineSegment1, const LineSegment2D& lineSegment2)
	{
		const LineSegment2DOrientation o1 = getOrientation(lineSegment1.begin, lineSegment1.end, lineSegment2.begin);
		const LineSegment2DOrientation o2 = getOrientation(lineSegment1.begin, lineSegment1.end, lineSegment2.end);
		const LineSegment2DOrientation o3 = getOrientation(lineSegment2.begin, lineSegment2.end, lineSegment1.begin);
		const LineSegment2DOrientation o4 = getOrientation(lineSegment2.begin, lineSegment2.end, lineSegment1.end);

		if (o1 != o2 && o3 != o4)
		{
			return true;
		}
		else if (o1 == LineSegment2DOrientation::colinear && onSegment(lineSegment1.begin, lineSegment2.begin, lineSegment1.end))
		{
			return true;
		}
		if (o2 == LineSegment2DOrientation::colinear && onSegment(lineSegment1.begin, lineSegment2.end, lineSegment1.end))
		{
			return true;
		}
		if (o3 == LineSegment2DOrientation::colinear && onSegment(lineSegment2.begin, lineSegment1.begin, lineSegment2.end))
		{
			return true;
		}
		if (o4 == LineSegment2DOrientation::colinear && onSegment(lineSegment2.begin, lineSegment1.end, lineSegment2.end))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}
