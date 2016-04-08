
#include "Camera3D.h"
#include "ApplicationData.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>


#define GLM_FORCE_RADIANS


namespace spehs
{
	Camera3D::Camera3D() : fov(45.0f), near(0.05f), far(500.0f), rotation(1.0f)
	{
		position = glm::vec3(0.0f, 0.0f, 3.0f);

		up = glm::vec3(0.0f, 1.0f, 0.0f);
		target = glm::vec3(0.0f, 0.0f, 0.0f);
		right = glm::vec3(1.0f, 0.0f, 0.0f);

		view = glm::mat4(1.0f);
		projection = glm::perspective(fov, (float)applicationData->getWindowWidth() / (float)applicationData->getWindowHeight(), near, far);

		cameraMatrix = &view;
	}
	Camera3D::~Camera3D()
	{
	}

	void Camera3D::update()
	{
		direction = glm::normalize(target - position);
		right = glm::cross(direction, up);
		glm::quat pitchQ = glm::angleAxis(rotation.x, right);
		glm::quat yawQ = glm::angleAxis(-rotation.y, up);
		glm::quat temp = glm::normalize(glm::cross(pitchQ, yawQ));
		direction = glm::rotate(temp, direction);
		target = position + direction;
		
		view = glm::lookAt(position, target, up);
		view = projection * view;
	}

	void Camera3D::moveForward(const float& _amount)
	{
		position += direction * _amount;
	}

	void Camera3D::moveBackward(const float& _amount)
	{
		position -= direction * _amount;
	}

	void Camera3D::moveRight(const float& _amount)
	{
		position += right * _amount;
	}

	void Camera3D::moveLeft(const float& _amount)
	{
		position -= right * _amount;
	}

	void Camera3D::moveUp(const float& _amount)
	{
		position += up * _amount;
	}

	void Camera3D::moveDown(const float& _amount)
	{
		position -= up * _amount;
	}

	void Camera3D::setPosition(const glm::vec3 &_position)
	{
		position = _position;
	}
	
	void Camera3D::yaw(const float& _yaw)
	{
		rotation.x = _yaw;
	}

	void Camera3D::pitch(const float& _pitch)
	{
		rotation.y = _pitch;
	}

	void Camera3D::setRotation(const glm::vec2 &_rotation)
	{
		rotation.x = _rotation.x;
		rotation.y = _rotation.y;
	}

	void Camera3D::setTarget(const glm::vec3& _target)
	{
		target = _target;
	}

	void Camera3D::setFOV(const float &_fov)
	{
		fov = _fov;
		projection = glm::perspective(fov, (float) applicationData->getWindowWidth() / (float) applicationData->getWindowHeight(), near, far);
	}
}