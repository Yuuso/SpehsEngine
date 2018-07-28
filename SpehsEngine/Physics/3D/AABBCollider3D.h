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

			void set(const glm::vec3& _min, const glm::vec3& _max);

			glm::vec3 min() const;
			glm::vec3 max() const;

			void setPosition(const glm::vec3& _position) { position = _position; }
			glm::vec3 getPosition() const { return position; }

		private:
			glm::vec3 minCorner;
			glm::vec3 maxCorner;
			glm::vec3 position;
		};
	}
}
