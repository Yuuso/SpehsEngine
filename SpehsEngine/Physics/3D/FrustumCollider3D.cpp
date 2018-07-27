
#include "stdafx.h"

#include "SpehsEngine/Physics/3D/FrustumCollider3D.h"

#include <glm/geometric.hpp>


namespace se
{
	namespace physics
	{
		FrustumCollider3D::FrustumCollider3D(const glm::vec3& _nearTopLeft,
												const glm::vec3& _nearTopRight,
												const glm::vec3& _nearBottomLeft,
												const glm::vec3& _nearBottomRight,
												const glm::vec3& _farTopLeft,
												const glm::vec3& _farTopRight,
												const glm::vec3& _farBottomLeft,
												const glm::vec3& _farBottomRight)
		{
			planes[Plane::TOP].construct(	_farTopLeft,		_farTopRight,		_nearTopRight);
			planes[Plane::BOTTOM].construct(_nearBottomLeft,	_nearBottomRight,	_farBottomRight);
			planes[Plane::LEFT].construct(	_farTopLeft,		_nearTopLeft,		_nearBottomLeft);
			planes[Plane::RIGHT].construct(	_nearTopRight,		_farTopRight,		_farBottomRight);
			planes[Plane::NEAR].construct(	_nearTopLeft,		_nearTopRight,		_nearBottomRight);
			planes[Plane::FAR].construct(	_farTopRight,		_farTopLeft,		_farBottomLeft);
		}

		void FrustumCollider3D::Plane::construct(const glm::vec3& _point1, const glm::vec3& _point2, const glm::vec3& _point3)
		{
			normal = glm::cross(_point2 - _point1, _point3 - _point1);
			normal = glm::normalize(normal);
			dot = -glm::dot(_point1, normal);
		}

		float FrustumCollider3D::Plane::signedDistance(const glm::vec3& _point) const
		{
			return (dot + glm::dot(normal, _point));
		}
	}
}
