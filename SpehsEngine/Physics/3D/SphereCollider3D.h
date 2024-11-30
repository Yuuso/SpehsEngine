#pragma once


namespace se
{
	namespace physics
	{
		class SphereCollider3D
		{
		public:
			SphereCollider3D();
			SphereCollider3D(const glm::vec3& _center, float _radius);

			glm::vec3 center;
			float radius;
		};
	}
}
