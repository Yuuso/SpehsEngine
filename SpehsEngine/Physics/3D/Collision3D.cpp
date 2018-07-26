
#include "stdafx.h"

#include "SpehsEngine/Physics/3D/Collision3D.h"
#include "SpehsEngine/Physics/3D/AABBCollider3D.h"
#include "SpehsEngine/Physics/3D/Ray3D.h"

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
				float t0 = (_aabb.min[i] - _ray.origin[i]) * invDirection;
				float t1 = (_aabb.max[i] - _ray.origin[i]) * invDirection;
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
	}
}