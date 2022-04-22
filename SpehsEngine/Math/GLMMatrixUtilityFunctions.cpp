#include "stdafx.h"
#include "SpehsEngine/Math/GLMMatrixUtilityFunctions.h"

#include "glm/mat3x3.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/matrix_decompose.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/transform.hpp"


namespace se
{
	glm::mat4 constructTransformationMatrix(const glm::vec3& _pos, const glm::quat& _rot, const glm::vec3& _scale)
	{
		return glm::translate(_pos) * glm::mat4_cast(_rot) * glm::scale(glm::vec3(_scale));
	}
	void decomposeTransformationMatrix(const glm::mat4& _transformationMatrix, glm::vec3* _pos, glm::quat* _rot, glm::vec3* _scale)
	{
		// TODO: Test properly, seems to at least have some floating point inaccuracies!

		glm::mat4 m = _transformationMatrix;

		if (_pos)
		{
			*_pos = glm::vec3(m[3][0], m[3][1], m[3][2]);
		}
		m[3][0] = m[3][1] = m[3][2] = 0.0f;

		if (_scale)
		{
			(*_scale).x = glm::length(glm::vec3(m[0][0], m[0][1], m[0][2]));
			(*_scale).y = glm::length(glm::vec3(m[1][0], m[1][1], m[1][2]));
			(*_scale).z = glm::length(glm::vec3(m[2][0], m[2][1], m[2][2]));
			m[0] /= (*_scale).x;
			m[1] /= (*_scale).y;
			m[2] /= (*_scale).z;
		}
		else if (_rot)
		{
			glm::vec3 dummy;
			dummy.x = glm::length(glm::vec3(m[0][0], m[0][1], m[0][2]));
			dummy.y = glm::length(glm::vec3(m[1][0], m[1][1], m[1][2]));
			dummy.z = glm::length(glm::vec3(m[2][0], m[2][1], m[2][2]));
			m[0] /= dummy.x;
			m[1] /= dummy.y;
			m[2] /= dummy.z;
		}

		if (_rot)
		{
			*_rot = glm::quat_cast(m);
		}
	}
	glm::mat4 constructNormalMatrix(const glm::mat4& _transformationMatrix)
	{
		return glm::mat4(glm::inverse(glm::transpose(glm::mat3(_transformationMatrix))));
	}
}
