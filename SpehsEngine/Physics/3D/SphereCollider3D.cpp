#include "stdafx.h"
#include "SpehsEngine/Physics/3D/SphereCollider3D.h"


namespace se
{
	namespace physics
	{
		SphereCollider3D::SphereCollider3D()
		{}
		SphereCollider3D::SphereCollider3D(const glm::vec3& _center, float _radius)
			: center(_center)
			, radius(_radius)
		{}
	}
}
