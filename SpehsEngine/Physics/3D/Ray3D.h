#pragma once


namespace se
{
	namespace physics
	{
		class Ray3D
		{
		public:
			Ray3D(const glm::vec3& _origin, const glm::vec3& _direction, const float _length);
			Ray3D(const glm::vec3& _point1, const glm::vec3& _point2);

			glm::vec3 origin;
			glm::vec3 direction;
			float length;
		};
	}
}
