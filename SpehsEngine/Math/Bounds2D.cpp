#include "stdafx.h"
#include "SpehsEngine/Math/Bounds2D.h"

namespace se
{

	Bounds2D::Bounds2D(const glm::vec2& _center, const glm::vec2& _extents)
		: center(_center)
		, extents(_extents)
	{
	
	}

	glm::vec2 Bounds2D::getCenter() const
	{

	}

	glm::vec2 Bounds2D::getExtents() const
	{

	}

	glm::vec2 Bounds2D::getMin() const
	{
		return center - extents;
	}

	glm::vec2 Bounds2D::getMax() const
	{
		return center + extents;
	}

	glm::vec2 Bounds2D::getSize() const
	{
		return 2.0f * extents;
	}

	bool Bounds2D::contains(const glm::vec2& position) const
	{
		return position.x >= center.x - extents.x
			&& position.x <= center.x + extents.x
			&& position.y >= center.y - extents.y
			&& position.y <= center.y + extents.y;
	}

	bool Bounds2D::contains(const Bounds2D& other) const
	{
		return other.center.x - other.extents.x >= center.x - extents.x
			&& other.center.x + other.extents.x <= center.x + extents.x
			&& other.center.y - other.extents.y >= center.y - extents.y
			&& other.center.y + other.extents.y <= center.y + extents.y;
	}

	bool Bounds2D::overlaps(const Bounds2D& other) const
	{
		return !(other.center.x - other.extents.x > center.x + extents.x
			|| other.center.x + other.extents.x < center.x - extents.x
			|| other.center.y - other.extents.y > center.y + extents.y
			|| other.center.y + other.extents.y < center.y - extents.y);
	}
}
