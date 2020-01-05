#pragma once

#include <glm/vec3.hpp>


namespace se
{
	namespace graphics
	{
		class Camera
		{
		public:

			enum Projection
			{
				perspective,
				orthographic
			};


								Camera();


			Projection			projectionGet() const;
			const glm::vec3&	positionGet() const;
			const glm::vec3		directionGet() const;
			const glm::vec3&	targetGet() const;
			const glm::vec3&	worldUpGet() const;
			float				zoomGet() const;
			float				fovGet() const;
			float				nearGet() const;
			float				farGet() const;

			void				projectionSet(const Projection _projection);
			void				positionSet(const glm::vec3& _position);
			void				directionSet(const glm::vec3& _direction);
			void				targetSet(const glm::vec3& _target);
			void				worldUpSet(const glm::vec3& _worldUp);
			void				zoomSet(const float _zoom);
			void				fovSet(const float _fov);
			void				nearSet(const float _near);
			void				farSet(const float _far);

		private:

			Projection			projection		= Projection::perspective;
			bool				followTarget	= false;

			glm::vec3			worldUp			= glm::vec3(0.0f, 1.0f, 0.0f);

			glm::vec3			position		= glm::vec3(0.0f, 10.0f, -15.0f);
			glm::vec3			target			= glm::vec3(0.0f, 0.0f, 0.0f);

			float				zoom			= 1.0f;
			float				fov				= 60.0f;
			float				nearPlane		= 0.05f;
			float				farPlane		= 1000.0f;
		};
	}
}
