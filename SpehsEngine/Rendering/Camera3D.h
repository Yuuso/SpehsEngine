
#pragma once

#include "SpehsEngine/Core/Vector.h"
#include "SpehsEngine/Core/Time.h"

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>


namespace spehs
{
	enum Movement
	{
		UP,
		DOWN,
		RIGHT,
		LEFT,
		FORWARD,
		BACKWARD,
	};

	class Window;

	class Camera3D
	{
	public:
		Camera3D(const Window& _window);
		~Camera3D();

		void initialize();
		void update(const time::Time& deltaTime);

		void move(const Movement& _movement, const float& _amount);
		void setSmoothCamera(const bool _value);
		void setMaxSpeed(const float& _value);

		void setPosition(const glm::vec3 &_position);

		void yaw(const float& _yaw);
		void pitch(const float& _pitch);
		//void roll(const float& _roll);
		void setRotation(const glm::vec2 &_rotation);

		void reset();

		void setTarget(const glm::vec3& _target);
		void setUp(const glm::vec3& _up);
		void setFOV(const float &_fov);
		void setNear(const float& _near);
		void setFar(const float& _far);

		glm::vec3 getPosition() const { return position; }
		glm::vec3 getDirection() const { return direction; }
		float getNear() const { return near; }
		float getFar() const { return far; }

		glm::mat4* cameraMatrix;

		const Window& window; // TODO: camera really doesn't depend on the window class...?

	private:
		bool smoothCamera;
		bool followTarget;

		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 movement;

		glm::vec3 direction;
		glm::vec3 target;
		glm::vec3 targetToFollow;
		glm::vec3 up;
		glm::vec3 right;

		glm::mat4 view;
		glm::mat4 projection;

		float fov;
		float near;
		float far;
		float maxSpeed;
	};
}