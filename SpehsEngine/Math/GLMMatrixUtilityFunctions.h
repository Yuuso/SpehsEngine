#pragma once


namespace se
{
	glm::mat4 constructTransformationMatrix(const glm::vec3& _pos, const glm::quat& _rot, const glm::vec3& _scale);
	void decomposeTransformationMatrix(const glm::mat4& _transformationMatrix, glm::vec3* _pos, glm::quat* _rot, glm::vec3* _scale);
	glm::mat4 constructNormalMatrix(const glm::mat4& _transformationMatrix);
}
