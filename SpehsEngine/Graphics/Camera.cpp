#include "stdafx.h"
#include "SpehsEngine/Graphics/Camera.h"


namespace se
{
	namespace graphics
	{
		Camera::Camera()
		{
		}

		Projection Camera::getProjection() const
		{
			return projection;
		}
		const glm::vec3& Camera::getPosition() const
		{
			return position;
		}
		const glm::vec3 Camera::getDirection() const
		{
			return glm::normalize(target - position);
		}
		const glm::vec3& Camera::getTarget() const
		{
			return target;
		}
		const glm::vec3& Camera::getUp() const
		{
			return up;
		}
		const glm::vec3 Camera::getLeft() const
		{
			return glm::normalize(glm::cross(up, getDirection()));
		}
		float Camera::getZoom() const
		{
			return zoom;
		}
		float Camera::getFov() const
		{
			return fov;
		}
		float Camera::getNear() const
		{
			return nearPlane;
		}
		float Camera::getFar() const
		{
			return farPlane;
		}

		void Camera::setProjection(const Projection _projection)
		{
			projection = _projection;
		}
		void Camera::setPosition(const glm::vec3& _position)
		{
			if (!followTarget)
				target = _position + getDirection();
			position = _position;
		}
		void Camera::setDirection(const glm::vec3& _direction)
		{
			se_assert(glm::normalize(_direction) != -up);
			followTarget = false;
			target = position + _direction;
		}
		void Camera::setTarget(const glm::vec3& _target)
		{
			se_assert(glm::normalize(_target) != -up);
			followTarget = true;
			target = _target;
		}
		void Camera::setUp(const glm::vec3& _up)
		{
			se_assert(glm::normalize(_up) != getDirection());
			up = glm::normalize(_up);
		}
		void Camera::setZoom(const float _zoom)
		{
			zoom = _zoom;
		}
		void Camera::setFov(const float _fov)
		{
			se_assert(_fov > 0.0f && _fov < 360.0f);
			fov = _fov;
		}
		void Camera::setNear(const float _near)
		{
			se_assert(_near > 0.0f && _near < farPlane);
			nearPlane = _near;
		}
		void Camera::setFar(const float _far)
		{
			se_assert(_far > nearPlane);
			farPlane = _far;
		}
	}
}