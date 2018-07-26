#pragma once

#include <glm/vec3.hpp>


namespace se
{
	namespace physics
	{
		class AABBCollider3D
		{
		public:
			AABBCollider3D(const glm::vec3& _min, const glm::vec3& _max);

			glm::vec3 min;
			glm::vec3 max;
		};
	}
}