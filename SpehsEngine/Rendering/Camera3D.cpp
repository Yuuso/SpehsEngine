
#include "stdafx.h"

#include "SpehsEngine/Rendering/Camera3D.h"
#include "SpehsEngine/Rendering/Window.h"

#include <glm/gtx/transform.hpp>


namespace se
{
	namespace rendering
	{
		Camera3D::Camera3D(const Window& _window)
			: fov(45.0f), near(0.05f), far(1000.0f), window(_window)
		{
			position = glm::vec3(0.0f);

			up = glm::vec3(0.0f, 1.0f, 0.0f);
			target = glm::vec3(0.0f, 0.0f, -1.0f);
			right = glm::vec3(1.0f, 0.0f, 0.0f);
			direction = glm::normalize(target - position);

			view = glm::mat4(1.0f);
			projection = glm::perspective(glm::radians(fov), (float)window.getWidth() / (float)window.getHeight(), near, far);

			cameraMatrix = &view;
		}

		void Camera3D::update(const time::Time& deltaTime)
		{
			view = glm::lookAt(position, target, up);
			view = projection * view;
		}

		void Camera3D::setPosition(const glm::vec3 &_position)
		{
			position = _position;
			target = position + direction;
		}
		void Camera3D::setTarget(const glm::vec3& _target)
		{
			direction = glm::normalize(_target - position);
			target = position + direction;
		}
		void Camera3D::setDirection(const glm::vec3& _direction)
		{
			direction = glm::normalize(_direction);
			target = position + direction;
		}

		void Camera3D::setUp(const glm::vec3& _up)
		{
			up = _up;
		}
		void Camera3D::setFOV(const float &_fov)
		{
			fov = _fov;
			projection = glm::perspective(glm::radians(fov), (float)window.getWidth() / (float)window.getHeight(), near, far);
		}
		void Camera3D::setNear(const float& _near)
		{
			near = _near;
			projection = glm::perspective(glm::radians(fov), (float)window.getWidth() / (float)window.getHeight(), near, far);
		}
		void Camera3D::setFar(const float& _far)
		{
			far = _far;
			projection = glm::perspective(glm::radians(fov), (float)window.getWidth() / (float)window.getHeight(), near, far);
		}
	}
}