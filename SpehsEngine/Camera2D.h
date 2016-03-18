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
		void enableCameraMatrix();
		void update();
		void translate(glm::vec2& vec) { worldPosition.translate(vec); }
		void translate(int64_t _ix, int64_t _iy) { worldPosition.translate(_ix, _iy); }

		//Setters
		void setScale(float newScale) { scale = newScale; }

		//Getters
		float getScale() { return scale; }
		WorldPosition& getWorldPositionRef() { return worldPosition; }
		
		//Public Variables
		glm::vec2 deltaMovement;
		glm::mat4* projectionMatrix;
		glm::mat4 staticMatrix;

	private:
		float zoomSpeed;
		float scale;
		glm::mat4 orthoMatrix;
		glm::mat4 cameraMatrix;
		WorldPosition worldPosition;
		WorldPosition previousPosition;
	};
}