#pragma once


namespace se
{
	struct Bounds2D
	{
		Bounds2D() = default;
		Bounds2D(const glm::vec2& _center, const glm::vec2& _extents);

		glm::vec2 getCenter() const;
		glm::vec2 getExtents() const;
		glm::vec2 getMin() const;
		glm::vec2 getMax() const;
		glm::vec2 getSize() const;
		bool contains(const glm::vec2& position) const;
		/* Returns true only if the other bounds are completely covered by this. */
		bool contains(const Bounds2D& other) const;
		bool checkOverlap(const Bounds2D& other) const;

		glm::vec2 center;
		glm::vec2 extents;
	};
}
