
#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>


namespace spehs
{
	class Camera3D
	{
	public:
		Camera3D();
		~Camera3D();
		
		void initialize();
		void update();

		void moveForward(const float& _amount);
		void moveBackward(const float& _amount);
		void moveRight(const float& _amount);
		void moveLeft(const float& _amount);
		void moveUp(const float& _amount);
		void moveDown(const float& _amount);

		void setPosition(const glm::vec3 &_position);

		void yaw(const float& _yaw);
		void pitch(const float& _pitch);
		//void roll(const float& _roll);
		void setRotation(const glm::vec2 &_rotation);

		void setTarget(const glm::vec3& _target);
		void setUp(const glm::vec3& _up);
		void setFOV(const float &_fov);

		glm::mat4* cameraMatrix;

	private:

		glm::vec3 position;
		glm::vec3 rotation;

		glm::vec3 direction;
		glm::vec3 target;
		glm::vec3 up;
		glm::vec3 right;

		glm::mat4 view;
		glm::mat4 projection;

		float fov;
		float near;
		float far;
	};
}