#include "stdafx.h"

#include "SpehsEngine/Rendering/Camera3D.h"
#include "SpehsEngine/Rendering/Window.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>


namespace se
{
	namespace rendering
	{
		Camera3D::Camera3D(const Window& _window)
			: window(_window)
			, fov(45.0f)
			, zNear(0.05f)
			, zFar(1000.0f)
		{
			position = glm::vec3(0.0f);

			up = glm::vec3(0.0f, 1.0f, 0.0f);
			target = glm::vec3(0.0f, 0.0f, -1.0f);
			right = glm::vec3(1.0f, 0.0f, 0.0f);
			direction = glm::normalize(target - position);

			view = glm::mat4(1.0f);
			updateProjectionMatrix();

			cameraMatrix = &view;
		}

		void Camera3D::updateProjectionMatrix()
		{
			if (viewProjection == PERSPECTIVE)
				projection = glm::perspective(glm::radians(fov), (float)window.getWidth() / (float)window.getHeight(), zNear, zFar);
			else if (viewProjection == ORTHOGRAPHIC)
				projection = glm::ortho(-(float)window.getWidth() * 0.01f, (float)window.getWidth() * 0.01f,
										-(float)window.getHeight() * 0.01f, (float)window.getHeight() * 0.01f, zNear, zFar);
			else se_assert(false);
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
			updateProjectionMatrix();
		}
		void Camera3D::setNear(const float& _near)
		{
			zNear = _near;
			updateProjectionMatrix();
		}
		void Camera3D::setFar(const float& _far)
		{
			zFar = _far;
			updateProjectionMatrix();
		}

		void Camera3D::setViewProjection(const ViewProjection _vp)
		{
			viewProjection = _vp;
			updateProjectionMatrix();
		}
		void Camera3D::toggleViewProjection()
		{
			viewProjection = viewProjection == PERSPECTIVE ? ORTHOGRAPHIC : PERSPECTIVE;
			updateProjectionMatrix();
		}

		glm::vec3 Camera3D::getFrustumPoint(const glm::vec3& _screenCoordinates) const
		{
			return glm::unProject(_screenCoordinates, glm::lookAt(position, target, up), projection, glm::ivec4(0, 0, window.getWidth(), window.getHeight()));
		}
		glm::vec2 Camera3D::worldToScreen(const glm::vec3& _worldCoordinates) const
		{
			glm::vec3 frustumCoordinates = glm::project(_worldCoordinates, glm::lookAt(position, target, up), projection, glm::ivec4(0, 0, window.getWidth(), window.getHeight()));
			return glm::vec2(frustumCoordinates.x, frustumCoordinates.y);
		}
	}
}