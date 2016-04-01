
#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>


namespace spehs
{
	class Camera3D
	{
	public:
		Camera3D();
		~Camera3D();
		
		void initialize();
		void update();

		void translate(const glm::vec3& _translation);

	private:
		glm::vec2 project(glm::vec3& _point3d);

		glm::vec3 position;
		glm::vec3 viewerPosition;
		glm::vec3 orientation;
	};
}