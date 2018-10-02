
#pragma once

#include "SpehsEngine/Core/Time.h"

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
			Camera3D(const Window& _window);

			void update(const time::Time& _deltaTime);

			void setPosition(const glm::vec3& _position);
			void setTarget(const glm::vec3& _target);
			void setDirection(const glm::vec3& _direction);

			void setUp(const glm::vec3& _up);
			void setFOV(const float &_fov);
			void setNear(const float& _near);
			void setFar(const float& _far);

			glm::vec3 getPosition() const { return position; }
			glm::vec3 getDirection() const { return direction; }
			glm::vec3 getTarget() const { return target; }
			float getNear() const { return zNear; }
			float getFar() const { return zFar; }
			float getFOV() const { return fov; }

			glm::vec3 getFrustumPoint(const glm::vec3& _screenCoordinates) const;
			glm::vec2 worldToScreen(const glm::vec3& _worldCoordinates) const;

			glm::mat4* cameraMatrix; // todo clean up

		private:
			const Window& window;

			glm::vec3 position;
			glm::vec3 direction;
			glm::vec3 target;

			glm::vec3 up;
			glm::vec3 right;

			glm::mat4 view;
			glm::mat4 projection;

			float fov;
			float zNear;
			float zFar;
		};
	}
}