
#include "Camera2D.h"
#include "ApplicationData.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec3.hpp>



namespace spehs
{
	Camera2D::Camera2D() : zoomSpeed(50.0f), scale(1.0f), orthoMatrix(1.0f), cameraMatrix(1.0f), position(0.0f, 0.0f), previousPosition(0.0f, 0.0f), rotation(0.0f), up(0.0f, 1.0f, 0.0f, 1.0f)
	{
		orthoMatrix = glm::ortho(0.0f, (float) spehs::ApplicationData::getWindowWidth(), 0.0f, (float) spehs::ApplicationData::getWindowHeight());
		staticMatrix = orthoMatrix;

		//Camera translation
		glm::vec3 translation(spehs::ApplicationData::getWindowWidth()*0.5f, spehs::ApplicationData::getWindowHeight()*0.5f, 0);
		cameraMatrix = glm::translate(orthoMatrix, translation);


		defaultMatrix = cameraMatrix;
		projectionMatrix = &cameraMatrix;
	}
	Camera2D::~Camera2D()
	{
	}


	void Camera2D::translate(const spehs::vec2& _vec)
	{
		position += _vec;
		//?rotation
	}

	void Camera2D::setRotation(const float &_rotation)
	{
		rotation = _rotation;
		//up = glm::normalize(glm::rotate(glm::mat4(), rotation, spehs::vec3(0.0f, 0.0f, 1.0f)) * up);
	}

	void Camera2D::update()
	{
		//1. Rotation
		cameraMatrix = glm::rotate(defaultMatrix, -rotation, glm::vec3(0.0f, 0.0f, 1.0f));
		
		//2. Scaling
		cameraMatrix = glm::scale(cameraMatrix, glm::vec3(scale, scale, 0.0f));

		//3. Translation
		cameraMatrix = glm::translate(cameraMatrix, glm::vec3(-position.x, -position.y, 0.0f));

		//Delta movement
		deltaMovement = position - previousPosition;
		previousPosition = position;
	}
}