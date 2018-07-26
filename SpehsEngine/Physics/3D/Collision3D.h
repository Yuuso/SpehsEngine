
#pragma once


namespace se
{
	namespace physics
	{
		class Ray3D;
		class AABBCollider3D;
		class Collision3D
		{
		public:
			Collision3D(const Ray3D& _ray, const AABBCollider3D& _aabb);

			bool hit() const { return collisionHit; };

		private:
			bool collisionHit;
		};
	}
}