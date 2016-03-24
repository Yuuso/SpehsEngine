#pragma once

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include "WorldPosition.h"


namespace spehs
{
	class Camera2D
	{
	public:
		Camera2D();
		~Camera2D();

		void initialize();
		void update();

		void translate(const glm::vec2& _vec);
				
		//Public Variables
		glm::vec2 deltaMovement;
		glm::mat4* projectionMatrix;
		glm::mat4 staticMatrix;

		float scale;
		glm::vec2 position;
		glm::vec2 previousPosition;

	private:
		float zoomSpeed;
		glm::mat4 orthoMatrix;
		glm::mat4 cameraMatrix;
	};
}