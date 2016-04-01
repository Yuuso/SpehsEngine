
#include "Camera3D.h"
#include "ApplicationData.h"

#include <glm/gtx/transform.hpp>


namespace spehs
{
	Camera3D::Camera3D() : fov(60.0f), near(0.1f), far(500.0f)
	{
		position = glm::vec3(0.0f, 0.0f, 3.0f);
		target = glm::vec3(0.0f);

		up = glm::vec3(0.0f, 1.0f, 0.0f);
		front = glm::vec3(0.0f, 0.0f, -1.0f);

		view = glm::mat4(1.0f);
		projection = glm::perspective(fov, (float)applicationData->getWindowWidth() / (float)applicationData->getWindowHeight(), near, far);

		cameraMatrix = &view;
	}
	Camera3D::~Camera3D()
	{
	}

	void Camera3D::update()
	{
		right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
		up = glm::normalize(glm::cross(right, front));
		direction = glm::normalize(position - target);
		view = glm::lookAt(position, position + front, up);
		view = projection * view;
	}

	void Camera3D::translate(const glm::vec3& _translation)
	{
		position += _translation;
	}

	void Camera3D::setPosition(const glm::vec3 &_position)
	{
		position = _position;
	}

	void Camera3D::rotate(const glm::vec2 &_rotation)
	{
		//TODO:?
	}

	void Camera3D::setRotation(const glm::vec2 &_rotation)
	{
		front = glm::normalize(glm::vec3(cos(_rotation.x * _rotation.y), sin(_rotation.x), cos(_rotation.x * _rotation.y)));
	}

	void Camera3D::setFOV(const float &_fov)
	{
		fov = _fov;
		projection = glm::perspective(fov, (float) applicationData->getWindowWidth() / (float) applicationData->getWindowHeight(), near, far);
	}
}