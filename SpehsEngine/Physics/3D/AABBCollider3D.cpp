
#include "stdafx.h"
#include "SpehsEngine/Physics/3D/AABBCollider3D.h"
#include "SpehsEngine/Core/SE_Assert.h"


namespace se
{
	namespace physics
	{
		AABBCollider3D::AABBCollider3D()
			: center(0.0f)
			, extents(0.0f)
		{
		}
		AABBCollider3D::AABBCollider3D(const glm::vec3& _center, const glm::vec3& _extents)
			: center(_center)
			, extents(_extents)
		{
		}
		AABBCollider3D AABBCollider3D::MinMax(const glm::vec3& _min, const glm::vec3& _max)
		{
			AABBCollider3D result;
			result.setMinMax(_min, _max);
			return result;
		}

		const glm::vec3 AABBCollider3D::getMin() const
		{
			return center - extents;
		}
		const glm::vec3 AABBCollider3D::getMax() const
		{
			return center + extents;
		}
		const glm::vec3 AABBCollider3D::getExtents() const
		{
			return extents;
		}
		const glm::vec3 AABBCollider3D::getCenter() const
		{
			return center;
		}

		void AABBCollider3D::setMinMax(const glm::vec3& _min, const glm::vec3& _max)
		{
			se_assert(_min.x <= _max.x && _min.y <= _max.y && _min.z <= _max.z);
			extents = glm::vec3(_max.x - _min.x, _max.y - _min.y, _max.z - _min.z) / 2.0f;
			center = { _max.x - extents.x, _max.y - extents.y, _max.z - extents.z };
		}
		void AABBCollider3D::setExtents(const glm::vec3& _extents)
		{
			extents = _extents;
		}
		void AABBCollider3D::setCenter(const glm::vec3& _center)
		{
			center = _center;
		}
	}
}
