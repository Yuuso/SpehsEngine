
#include "stdafx.h"
#include "SpehsEngine/Physics/3D/AABBCollider3D.h"
#include "SpehsEngine/Core/Assert.h"


namespace se
{
	namespace physics
	{
		AABBCollider3D::AABBCollider3D(const glm::vec3& _min, const glm::vec3& _max)
			: min(_min)
			, max(_max)
		{
			se_assert(_min.x < _max.x && _min.y < _max.y && _min.z < _max.z);
		}
	}
}