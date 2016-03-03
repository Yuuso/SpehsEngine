#include <glm/mat3x3.hpp>
#include "ApplicationData.h"
#include "Camera3D.h"

namespace spehs
{
	Camera3D* camera3D;

	Camera3D::Camera3D()
	{
	}


	Camera3D::~Camera3D()
	{
	}

	void Camera3D::update()
	{

	}
	glm::vec2 Camera3D::project(glm::vec3& point)
	{

		/*
		????:
		https://en.wikipedia.org/wiki/3D_projection
		*/
		glm::mat3 mat1;
		mat1[0][0] = 1;	mat1[0][1] = 0;						mat1[0][2] = 0;
		mat1[1][0] = 0;	mat1[1][1] = cos(-camera3D->orientation.x);	mat1[1][2] = -sin(-orientation.x);
		mat1[2][0] = 0;	mat1[2][1] = sin(-orientation.x);	mat1[2][2] = cos(-orientation.x);

		glm::mat3 mat2;
		mat2[0][0] = cos(-orientation.y);	mat2[0][1] = 0;		mat2[0][2] = sin(-orientation.y);
		mat2[1][0] = 0;						mat2[1][1] = 1;		mat2[1][2] = 0;
		mat2[2][0] = -sin(-orientation.y);	mat2[2][1] = 0;		mat2[2][2] = cos(-orientation.y);

		glm::mat3 mat3;
		mat3[0][0] = cos(-orientation.z);	mat3[0][1] = -sin(-orientation.z);	mat3[0][2] = 0;
		mat3[1][0] = sin(-orientation.z);	mat3[1][1] = cos(-orientation.z);	mat3[1][2] = 0;
		mat3[2][0] = 0;						mat3[2][1] = 0;						mat3[2][2] = 1;

		glm::vec3 d = mat1*mat2*mat3*(point - position);

		if (d.z != 0)
		{
			glm::vec2 projectedPoint = glm::vec2((viewerPosition.z / d.z)*d.x*(float(applicationData->getWindowHeight()) / applicationData->getWindowWidth()), (viewerPosition.z / d.z)*d.y);
			return projectedPoint;
		}

		return glm::vec2(0, 0);
	}
}