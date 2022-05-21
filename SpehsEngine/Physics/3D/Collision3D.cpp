#include "stdafx.h"
#include "SpehsEngine/Physics/3D/Collision3D.h"

#include "SpehsEngine/Physics/3D/AABBCollider3D.h"
#include "SpehsEngine/Physics/3D/FrustumCollider3D.h"
#include "SpehsEngine/Physics/3D/Ray3D.h"
#include "SpehsEngine/Physics/3D/SphereCollider3D.h"
#include <algorithm>
#include <limits>
#include <glm/gtx/norm.hpp>


namespace se
{
	namespace physics
	{
		Collision3D::Collision3D(const Ray3D& _ray, const AABBCollider3D& _aabb)
		{
			collisionHit = true;
			float tmin = 0.0f, tmax = _ray.length;
			for (size_t i = 0; i < 3; i++)
			{
				const float invDirection = 1.0f / _ray.direction[glm::vec2::length_type(i)];
				float t0 = (_aabb.getMin()[glm::vec2::length_type(i)] - _ray.origin[glm::vec2::length_type(i)]) * invDirection;
				float t1 = (_aabb.getMax()[glm::vec2::length_type(i)] - _ray.origin[glm::vec2::length_type(i)]) * invDirection;
				if (invDirection < 0.0f)
					std::swap(t0, t1);
				tmin = t0 > tmin ? t0 : tmin;
				tmax = t1 < tmax ? t1 : tmax;
				if (tmax < tmin)
				{
					collisionHit = false;
					break;
				}
			}
			if (collisionHit)
			{
				collisionPoint = _ray.origin + _ray.direction * tmin;
			}
		}

		Collision3D::Collision3D(const Ray3D& _ray, const SphereCollider3D& _sphere)
		{
			const glm::vec3 directionToSphere = _sphere.center - _ray.origin;
			const float rayDistanceToMiddle = glm::dot(directionToSphere, _ray.direction);
			const float sqrLength = glm::length2(directionToSphere);
			if ((_sphere.radius * _sphere.radius - sqrLength + rayDistanceToMiddle * rayDistanceToMiddle) < 0.0f)
			{
				collisionHit = false;
			}
			else
			{
				collisionHit = true;
				const float sphereDistanceToMiddle = glm::sqrt(sqrLength - (rayDistanceToMiddle * rayDistanceToMiddle));
				const float middleDistanceToHit = glm::sqrt((_sphere.radius * _sphere.radius) - (sphereDistanceToMiddle * sphereDistanceToMiddle));

				if (sqrLength < (_sphere.radius * _sphere.radius))
				{
					collisionPoint = _ray.origin + _ray.direction * (rayDistanceToMiddle + middleDistanceToHit);
				}
				else
				{
					collisionPoint = _ray.origin + _ray.direction * (rayDistanceToMiddle - middleDistanceToHit);
				}
			}
		}

		Collision3D::Collision3D(const FrustumCollider3D& _frustum, const AABBCollider3D& _aabb)
		{
			collisionInfo = (int)FrustumCollider3D::INSIDE;
			for (size_t i = 0; i < 6; i++)
			{
				if (_frustum.planes[i].signedDistance(getNegativeVertex(_aabb, _frustum.planes[i].normal)) > 0)
				{
					collisionInfo = (int)FrustumCollider3D::OUTSIDE;
					break;
				}
				else if (_frustum.planes[i].signedDistance(getPositiveVertex(_aabb, _frustum.planes[i].normal)) > 0)
					collisionInfo = (int)FrustumCollider3D::INTERSECT;
			}
			collisionHit = collisionInfo != (int)FrustumCollider3D::OUTSIDE;
		}
		glm::vec3 Collision3D::getPositiveVertex(const AABBCollider3D& _aabb, const glm::vec3& _normal)
		{
			glm::vec3 result;
			result.x = (_normal.x > 0.0f) ? _aabb.getMax().x : _aabb.getMin().x;
			result.y = (_normal.y > 0.0f) ? _aabb.getMax().y : _aabb.getMin().y;
			result.z = (_normal.z > 0.0f) ? _aabb.getMax().z : _aabb.getMin().z;
			return result;
		}
		glm::vec3 Collision3D::getNegativeVertex(const AABBCollider3D& _aabb, const glm::vec3& _normal)
		{
			glm::vec3 result;
			result.x = (_normal.x < 0.0f) ? _aabb.getMax().x : _aabb.getMin().x;
			result.y = (_normal.y < 0.0f) ? _aabb.getMax().y : _aabb.getMin().y;
			result.z = (_normal.z < 0.0f) ? _aabb.getMax().z : _aabb.getMin().z;
			return result;
		}

		Collision3D::Collision3D(const FrustumCollider3D& _frustum, const glm::vec3& _point)
		{
			collisionHit = true;
			for (size_t i = 0; i < 6; i++)
			{
				if (_frustum.planes[i].signedDistance(_point) > 0)
				{
					collisionHit = false;
					break;
				}
			}
		}
	}
}
