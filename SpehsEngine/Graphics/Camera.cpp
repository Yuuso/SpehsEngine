#include "stdafx.h"
#include "SpehsEngine/Graphics/Camera.h"


namespace se
{
	namespace graphics
	{
		Camera::Camera()
		{
		}

		Camera::Projection Camera::projectionGet() const
		{
			return projection;
		}
		const glm::vec3& Camera::positionGet() const
		{
			return position;
		}
		const glm::vec3 Camera::directionGet() const
		{
			return glm::normalize(target - position);
		}
		const glm::vec3& Camera::targetGet() const
		{
			return target;
		}
		const glm::vec3& Camera::worldUpGet() const
		{
			return worldUp;
		}
		float Camera::zoomGet() const
		{
			return zoom;
		}
		float Camera::fovGet() const
		{
			return fov;
		}
		float Camera::nearGet() const
		{
			return nearPlane;
		}
		float Camera::farGet() const
		{
			return farPlane;
		}

		void Camera::projectionSet(const Projection _projection)
		{
			projection = _projection;
		}
		void Camera::positionSet(const glm::vec3& _position)
		{
			if (!followTarget)
				target = _position + (target - position);
			position = _position;
		}
		void Camera::directionSet(const glm::vec3& _direction)
		{
			followTarget = false;
			target = position + _direction;
		}
		void Camera::targetSet(const glm::vec3& _target)
		{
			followTarget = true;
			target = _target;
		}
		void Camera::worldUpSet(const glm::vec3& _worldUp)
		{
			worldUp = _worldUp;
		}
		void Camera::zoomSet(const float _zoom)
		{
			zoom = _zoom;
		}
		void Camera::fovSet(const float _fov)
		{
			fov = _fov;
		}
		void Camera::nearSet(const float _near)
		{
			nearPlane = _near;
		}
		void Camera::farSet(const float _far)
		{
			farPlane = _far;
		}
	}
}