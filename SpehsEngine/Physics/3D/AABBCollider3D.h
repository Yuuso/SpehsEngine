#pragma once

#include <glm/vec3.hpp>


namespace se
{
	namespace physics
	{
		class AABBCollider3D
		{
		public:
			AABBCollider3D();
			AABBCollider3D(const glm::vec3& _center, const glm::vec3& _extents);
			static AABBCollider3D MinMax(const glm::vec3& _min, const glm::vec3& _max);

			const glm::vec3 getMin() const;
			const glm::vec3 getMax() const;
			const glm::vec3 getExtents() const;
			const glm::vec3 getCenter() const;

			void setMinMax(const glm::vec3& _min, const glm::vec3& _max);
			void setExtents(const glm::vec3& _extents);
			void setCenter(const glm::vec3& _center);

		private:
			glm::vec3 center;
			glm::vec3 extents;
		};
	}
}
