#include <glm/gtc/matrix_transform.hpp>
#include "Camera2D.h"
#include "ApplicationData.h"


namespace spehs
{
	Camera2D::Camera2D() : zoomSpeed(100.0f),scale(1.0f), orthoMatrix(1.0f), cameraMatrix(1.0f), worldPosition(0, 0), previousPosition(0, 0){}
	Camera2D::~Camera2D(){}
	void Camera2D::initialize()
	{
		//Init
		orthoMatrix = glm::ortho(0.0f, (float) applicationData->getWindowWidth(), 0.0f, (float) applicationData->getWindowHeight());

		//Camera translation
		glm::vec3 translation(applicationData->getWindowWidth()*0.5f, applicationData->getWindowHeight()*0.5f, 0);
		cameraMatrix = glm::translate(orthoMatrix, translation);

		//Camera Scale
		glm::vec3 scaleVec(((float) applicationData->getWindowWidth() / (float) applicationData->getWindowHeight()) * scale, scale, 0.0f);
		cameraMatrix = glm::scale(glm::mat4(1.0f), scaleVec) * cameraMatrix;
		staticMatrix = cameraMatrix;
	}
	void Camera2D::enableCameraMatrix()
	{
		//Set camera matrix pointer
		projectionMatrix = &cameraMatrix;
	}
	void Camera2D::update()
	{//We could assume that the camera needs updating on each frame


		//Camera translation
		static glm::vec3 translation;
		translation = glm::vec3(applicationData->getWindowWidth()*0.5f, applicationData->getWindowHeight()*0.5f, 0);
		cameraMatrix = glm::translate(orthoMatrix, translation);

		//Camera Scale
		static glm::vec3 scaleVec;
		scaleVec = glm::vec3(((float) applicationData->getWindowWidth() / (float) applicationData->getWindowHeight()) * scale, scale, 0.0f);
		cameraMatrix = glm::scale(glm::mat4(1.0f), scaleVec) * cameraMatrix;

		//?
		enableCameraMatrix();

		//Delta movement
		deltaMovement = worldPosition - previousPosition;
		previousPosition = worldPosition;
	}


}