
#pragma once


namespace se
{
	namespace physics
	{
		class Ray3D;
		class AABBCollider3D;
		class FrustumCollider3D;
		class Collision3D
		{
		public:
			Collision3D(const Ray3D& _ray, const AABBCollider3D& _aabb);
			Collision3D(const FrustumCollider3D& _frustum, const AABBCollider3D& _aabb);
			Collision3D(const FrustumCollider3D& _frustum, const glm::vec3& _point);

			bool hit() const { return collisionHit; };
			int info() const { return collisionInfo; }

		private:
			inline glm::vec3 getPositiveVertex(const AABBCollider3D& _aabb, const glm::vec3& _normal);
			inline glm::vec3 getNegativeVertex(const AABBCollider3D& _aabb, const glm::vec3& _normal);

			bool collisionHit;
			int collisionInfo;
		};
	}
}
