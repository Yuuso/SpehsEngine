
#include "SpehsEngine/Rendering/Camera3D.h"
#include "SpehsEngine/Rendering/Window.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>


#define GLM_FORCE_RADIANS


namespace spehs
{
	Camera3D::Camera3D(const Window& _window)
		: fov(45.0f), near(0.05f), far(500.0f), rotation(glm::vec3(0.0f)), smoothCamera(false), maxSpeed(1.5f), followTarget(false), window(_window)
	{
		position = glm::vec3(1.0f, 1.0f, 1.0f);
		movement = glm::vec3(0.0f);

		up = glm::vec3(0.0f, 1.0f, 0.0f);
		target = glm::vec3(0.0f, 0.0f, 0.0f);
		right = glm::vec3(1.0f, 0.0f, 0.0f);
		direction = glm::normalize(target - position);

		view = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(fov), (float)window.getWidth() / (float)window.getHeight(), near, far);

		cameraMatrix = &view;
	}
	Camera3D::~Camera3D()
	{
	}

	void Camera3D::update(const time::Time& deltaTime)
	{
		//movement
		//TODO: possibly fix these?
		//if (movement.x > maxSpeed)
		//	movement.x = maxSpeed;
		//if (movement.y > maxSpeed)
		//	movement.y = maxSpeed;
		//if (movement.z > maxSpeed)
		//	movement.z = maxSpeed;
		position += movement;
		target = position + direction;

		if (smoothCamera)
			movement *= 0.95f - deltaTime.asSeconds();
		else
			movement = glm::vec3(0.0f);

		direction = glm::normalize(target - position);
		right = glm::cross(direction, up);
		glm::quat pitchQ = glm::angleAxis(rotation.x, right);
		glm::quat yawQ = glm::angleAxis(-rotation.y, up);
		rotation = glm::vec3(0.0f); //clear rotation
		direction = glm::normalize(glm::rotate(glm::normalize(glm::cross(pitchQ, yawQ)), direction));
		target = position + direction;
		if (followTarget)
		{
			target = targetToFollow;
			followTarget = false;
		}

		view = glm::lookAt(position, target, up);
		view = projection * view;
	}

	void Camera3D::move(const Movement& _movement, const float& _amount)
	{
		switch (_movement)
		{
		case FORWARD:
			movement += direction * _amount;
			break;
		case BACKWARD:
			movement -= direction * _amount;
			break;
		case UP:
			movement += up * _amount;
			break;
		case DOWN:
			movement -= up * _amount;
			break;
		case RIGHT:
			movement += right * _amount;
			break;
		case LEFT:
			movement -= right * _amount;
			break;
		default:
			break;
		}
	}

	void Camera3D::reset()
	{
		position = glm::vec3(1.0f, 1.0f, 1.0f);
		movement = glm::vec3(0.0f);

		up = glm::vec3(0.0f, 1.0f, 0.0f);
		target = glm::vec3(0.0f, 0.0f, 0.0f);
		right = glm::vec3(1.0f, 0.0f, 0.0f);
		direction = glm::normalize(target - position);
	}

	void Camera3D::setSmoothCamera(const bool _value)
	{
		smoothCamera = _value;
	}
	void Camera3D::setMaxSpeed(const float& _value)
	{
		maxSpeed = _value;
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
		targetToFollow = _target;
		followTarget = true;
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