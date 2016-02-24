#pragma once
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

namespace SpehsEngine
{
	class Camera3D
	{
	public:
		Camera3D();
		~Camera3D();
		
		void update();
		glm::vec2 project(glm::vec3& point3d);

	private:
		glm::vec3 position;
		glm::vec3 viewerPosition;
		glm::vec3 orientation;
	};
}