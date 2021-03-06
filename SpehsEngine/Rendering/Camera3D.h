
#pragma once

#include "SpehsEngine/Core/SE_Time.h"

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>


namespace se
{
	namespace rendering
	{
		class Window;
		class Camera3D
		{
		public:
			enum ViewProjection
			{
				PERSPECTIVE,
				ORTHOGRAPHIC
			};

			Camera3D(const Window& _window);

			void update(const time::Time& _deltaTime);

			void setPosition(const glm::vec3& _position);
			void setTarget(const glm::vec3& _target);
			void setDirection(const glm::vec3& _direction);

			void setUp(const glm::vec3& _up);
			void setFOV(const float &_fov);
			void setNear(const float& _near);
			void setFar(const float& _far);

			void setViewProjection(const ViewProjection _vp);
			void toggleViewProjection();
			void setOrthographicZoom(const float _value);

			const glm::vec3& getPosition() const { return position; }
			const glm::vec3& getDirection() const { return direction; }
			const glm::vec3& getTarget() const { return target; }
			float getNear() const { return zNear; }
			float getFar() const { return zFar; }
			float getFOV() const { return fov; }
			ViewProjection getViewProjection() const { return viewProjection; };
			float getOrthographicZoom() const { return orthographicZoom; }

			// In world space
			const glm::vec3& getWorldUp() const;
			const glm::vec3& getWorldRight() const;

			glm::vec3 getFrustumPoint(const glm::vec3& _screenCoordinates) const;
			glm::vec2 worldToScreen(const glm::vec3& _worldCoordinates) const;

			glm::mat4* cameraMatrix; // todo clean up

		private:
			const Window& window;

			ViewProjection viewProjection = PERSPECTIVE;
			void updateProjectionMatrix();

			glm::vec3 position;
			glm::vec3 direction;
			glm::vec3 target;

			glm::vec3 up;
			glm::vec3 right;

			glm::vec3 worldUp;
			glm::vec3 worldRight;

			glm::mat4 view;
			glm::mat4 projection;

			float orthographicZoom = 1.0f;

			float fov;
			float zNear;
			float zFar;
		};
	}
}