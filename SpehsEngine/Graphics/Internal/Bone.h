#pragma once


namespace se
{
	namespace graphics
	{
		class ModelNode;
		struct Bone
		{
			Bone(const ModelNode* _boneNode, const glm::mat4& _offsetMatrix)
				: boneNode(_boneNode), offsetMatrix(_offsetMatrix) {}
			const ModelNode* boneNode = nullptr;
			glm::mat4 offsetMatrix;
		};
	}
}
