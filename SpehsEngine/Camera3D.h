
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

		void translate(const glm::vec3 &_translation); //x = Right, y = Up, z = Back
		void setPosition(const glm::vec3 &_position);

		void rotate(const glm::vec2 &_rotation); //x = Pitch, y = Yaw
		void setRotation(const glm::vec2 &_rotation);

		void setFOV(const float &_fov);

		glm::mat4* cameraMatrix;

	private:

		glm::vec3 position;
		glm::vec3 target;
		glm::vec3 direction; //Vector from target to camera

		glm::vec3 up;
		glm::vec3 right;
		glm::vec3 front;

		glm::mat4 view;
		glm::mat4 projection;

		float fov;
		float near;
		float far;
	};
}