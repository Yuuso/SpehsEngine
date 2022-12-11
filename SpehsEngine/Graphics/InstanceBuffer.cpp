#include "stdafx.h"
#include "SpehsEngine/Graphics/InstanceBuffer.h"


namespace se
{
	namespace graphics
	{
		glm::vec3 TransformInstanceData::getPosition() const
		{
			glm::vec3 result;
			deconstruct(&result, nullptr, nullptr);
			return result;
		}
		glm::quat TransformInstanceData::getRotation() const
		{
			glm::quat result;
			deconstruct(nullptr, &result, nullptr);
			return result;
		}
		glm::vec3 TransformInstanceData::getScale() const
		{
			glm::vec3 result;
			deconstruct(nullptr, nullptr, &result);
			return result;
		}

		void TransformInstanceData::setPosition(const glm::vec3& _position)
		{
			glm::vec3 scale;
			glm::quat rotation;
			deconstruct(nullptr, &rotation, &scale);
			construct(_position, rotation, scale);
		}
		void TransformInstanceData::setRotation(const glm::quat& _rotation)
		{
			glm::vec3 scale;
			glm::vec3 position;
			deconstruct(&position, nullptr, &scale);
			construct(position, _rotation, scale);
		}
		void TransformInstanceData::setScale(const glm::vec3& _scale)
		{
			glm::vec3 position;
			glm::quat rotation;
			deconstruct(&position, &rotation, nullptr);
			construct(position, rotation, _scale);
		}

		void TransformInstanceData::construct(const glm::vec3& _position, const glm::quat& _rotation, const glm::vec3& _scale)
		{
			transformMatrix = constructTransformationMatrix(_position, _rotation, _scale);
		}
		void TransformInstanceData::deconstruct(glm::vec3* _position, glm::quat* _rotation, glm::vec3* _scale) const
		{
			decomposeTransformationMatrix(transformMatrix, _position, _rotation, _scale);
		}
	}
}
