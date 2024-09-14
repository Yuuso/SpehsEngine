#pragma once


namespace se
{
	struct LineSegment2D
	{
		LineSegment2D() = default;
		LineSegment2D(const glm::vec2& _begin, const glm::vec2& _end) : begin(_begin), end(_end) {}
		glm::vec2 begin;
		glm::vec2 end;
	};
}
