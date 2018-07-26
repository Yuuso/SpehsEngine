
#include "stdafx.h"
#include "SpehsEngine/Physics/3D/Ray3D.h"

#include <glm/geometric.hpp>
#include <glm/gtc/constants.hpp>


namespace se
{
	namespace physics
	{
		Ray3D::Ray3D(const glm::vec3 & _origin, const glm::vec3 & _direction, const float _length)
			: origin(_origin)
			, direction(_direction)
			, length(_length)
		{
		}
		Ray3D::Ray3D(const glm::vec3& _point1, const glm::vec3& _point2)
		{
			origin = _point1;
			direction = glm::normalize(_point2 - _point1);
			length = glm::length(_point2 - _point1);
		}
	}
}