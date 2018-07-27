
#include "stdafx.h"

#include "SpehsEngine/Physics/3D/Collision3D.h"
#include "SpehsEngine/Physics/3D/AABBCollider3D.h"
#include "SpehsEngine/Physics/3D/Ray3D.h"
#include "SpehsEngine/Physics/3D/FrustumCollider3D.h"

#include <algorithm>
#include <limits>


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
				const float invDirection = 1.0f / _ray.direction[i];
				float t0 = (_aabb.min()[i] - _ray.origin[i]) * invDirection;
				float t1 = (_aabb.max()[i] - _ray.origin[i]) * invDirection;
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
			result.x = (_normal.x > 0.0f) ? _aabb.max().x : _aabb.min().x;
			result.y = (_normal.y > 0.0f) ? _aabb.max().y : _aabb.min().y;
			result.z = (_normal.z > 0.0f) ? _aabb.max().z : _aabb.min().z;
			return result;
		}
		glm::vec3 Collision3D::getNegativeVertex(const AABBCollider3D& _aabb, const glm::vec3& _normal)
		{
			glm::vec3 result;
			result.x = (_normal.x < 0.0f) ? _aabb.max().x : _aabb.min().x;
			result.y = (_normal.y < 0.0f) ? _aabb.max().y : _aabb.min().y;
			result.z = (_normal.z < 0.0f) ? _aabb.max().z : _aabb.min().z;
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
