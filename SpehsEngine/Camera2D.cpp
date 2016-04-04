
#include "Camera2D.h"
#include "ApplicationData.h"

#include <glm/gtc/matrix_transform.hpp>


#include <iostream>
namespace spehs
{
	Camera2D::Camera2D() : zoomSpeed(50.0f), scale(1.0f), orthoMatrix(1.0f), cameraMatrix(1.0f), position(0.0f, 0.0f), previousPosition(0.0f, 0.0f), rotation(0.0f)
	{
		orthoMatrix = glm::ortho(0.0f, (float) applicationData->getWindowWidth(), 0.0f, (float) applicationData->getWindowHeight());

		//Camera translation
		glm::vec3 translation(applicationData->getWindowWidth()*0.5f, applicationData->getWindowHeight()*0.5f, 0);
		cameraMatrix = glm::translate(orthoMatrix, translation);

		staticMatrix = cameraMatrix;
		projectionMatrix = &cameraMatrix;
	}
	Camera2D::~Camera2D()
	{
	}


	void Camera2D::translate(const glm::vec2& _vec)
	{
		position += _vec;
	}

	void Camera2D::setRotation(const float &_rotation)
	{
		rotation = _rotation;
	}

	void Camera2D::update()
	{
		//Camera translation
		cameraMatrix = glm::rotate(orthoMatrix, -rotation, glm::vec3(0.0f, 0.0f, 1.0f));
		cameraMatrix = glm::translate(cameraMatrix, glm::vec3(-position.x, -position.y, 0.0f));

		//Camera Scale
		cameraMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, 0.0f)) * cameraMatrix;

		//Delta movement
		deltaMovement = position - previousPosition;
		previousPosition = position;
	}
}