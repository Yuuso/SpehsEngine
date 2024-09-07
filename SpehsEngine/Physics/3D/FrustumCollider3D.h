#pragma once


namespace se
{
	namespace physics
	{
		class FrustumCollider3D
		{
		public:
			FrustumCollider3D(const glm::vec3& _nearTopLeft,
								const glm::vec3& _nearTopRight,
								const glm::vec3& _nearBottomLeft,
								const glm::vec3& _nearBottomRight,
								const glm::vec3& _farTopLeft,
								const glm::vec3& _farTopRight,
								const glm::vec3& _farBottomLeft,
								const glm::vec3& _farBottomRight);

			class Plane
			{
			public:
				enum
				{
					_TOP = 0,
					_BOTTOM,
					_LEFT,
					_RIGHT,
					_NEAR, // when certain (unknown) conditions are met, the NEAR name doesn't compile and an underscore is required
					_FAR
				};

				void construct(const glm::vec3& _point1, const glm::vec3& _point2, const glm::vec3& _point3);
				float signedDistance(const glm::vec3& _point) const;

				glm::vec3 normal;
				float dot;
			};
			Plane planes[6];

			enum
			{
				INSIDE,
				OUTSIDE,
				INTERSECT
			};
		};
	}
}
