#include "stdafx.h"
#include "SpehsEngine/Graphics/Internal/Mesh.h"

#include "SpehsEngine/Graphics/Internal/ModelNode.h"
#include "SpehsEngine/Graphics/Model.h"


namespace se
{
	namespace graphics
	{
		Mesh::Mesh(const Model& _model, const ModelNode& _node, const size_t _materialIndex, std::shared_ptr<VertexBuffer> _vertexBuffer, std::shared_ptr<IndexBuffer> _indexBuffer)
			: model(_model)
			, node(_node)
			, materialIndex(_materialIndex)
		{
			setVertices(_vertexBuffer);
			setIndices(_indexBuffer);

			combinedTransformMatrices.resize(1);
			combinedNormalMatrices.resize(1);
		}

		void Mesh::update()
		{
			Primitive::update();
		}
		void Mesh::updateMatrices()
		{
			if (bones.empty())
			{
				Primitive::updateMatrices();
				combinedTransformMatrices[0] = Primitive::getTransformMatrices()[0] * node.getTransform();
				combinedNormalMatrices[0] = glm::mat4(glm::inverse(glm::transpose(glm::mat3(combinedTransformMatrices[0]))));
			}
			else
			{
				for (size_t boneIndex = 0; boneIndex < bones.size(); boneIndex++)
				{
					combinedTransformMatrices[boneIndex] = (Primitive::getTransformMatrices()[0] * bones[boneIndex].boneNode->getTransform()) * bones[boneIndex].offsetMatrix;
					combinedNormalMatrices[boneIndex] = glm::mat4(glm::inverse(glm::transpose(glm::mat3(combinedTransformMatrices[boneIndex]))));
				}
			}
		}

		const UniformMatrices& Mesh::getTransformMatrices() const
		{
			return combinedTransformMatrices;
		}
		const UniformMatrices& Mesh::getNormalMatrices() const
		{
			return combinedNormalMatrices;
		}

		std::shared_ptr<Material> Mesh::getMaterial() const
		{
			return model.getMaterial(materialIndex);
		}

		void Mesh::addBone(const ModelNode* _boneNode, const glm::mat4& _offsetMatrix)
		{
			se_assert(_boneNode);
			bones.emplace_back(_boneNode, _offsetMatrix);
			combinedTransformMatrices.resize(bones.size());
			combinedNormalMatrices.resize(bones.size());
		}
	}
}
