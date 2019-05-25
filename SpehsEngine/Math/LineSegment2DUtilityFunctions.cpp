/*
	Source: https://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/
*/
#include "stdafx.h"
#include "SpehsEngine/Math/LineSegment2DUtilityFunctions.h"

#include <algorithm>
#include "SpehsEngine/Math/LineSegment2D.h"
#include "SpehsEngine/Math/Bounds2D.h"

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

	bool checkOverlap(const LineSegment2D& lineSegment, const Bounds2D& bounds2D)
	{
		//Check if the line segment is inside the bounds
		if (bounds2D.contains(lineSegment.begin) || bounds2D.contains(lineSegment.end))
		{
			return true;
		}

		//Check if the line segments overlaps with any of the bounds' edges
		const glm::vec2 boundsVertices[4] =
		{
			glm::vec2(bounds2D.center.x - bounds2D.extents.x, bounds2D.center.y - bounds2D.extents.y),
			glm::vec2(bounds2D.center.x - bounds2D.extents.x, bounds2D.center.y + bounds2D.extents.y),
			glm::vec2(bounds2D.center.x + bounds2D.extents.x, bounds2D.center.y + bounds2D.extents.y),
			glm::vec2(bounds2D.center.x + bounds2D.extents.x, bounds2D.center.y - bounds2D.extents.y)
		};
		const LineSegment2D boundsLineSegments[4] =
		{
			LineSegment2D(boundsVertices[0], boundsVertices[1]),
			LineSegment2D(boundsVertices[1], boundsVertices[2]),
			LineSegment2D(boundsVertices[2], boundsVertices[3]),
			LineSegment2D(boundsVertices[3], boundsVertices[0])
		};
		for (size_t i = 0; i < 4; i++)
		{
			if (checkOverlap(lineSegment, boundsLineSegments[i]))
			{
				return true;
			}
		}

		return false;
	}
}
