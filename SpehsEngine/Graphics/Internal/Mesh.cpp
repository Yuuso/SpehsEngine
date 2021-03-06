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
		}

		void Mesh::update()
		{
			Primitive::update();
		}
		void Mesh::updateMatrices()
		{
			transformMatrix = node.getTransform();
			normalMatrix = glm::mat4(glm::inverse(glm::transpose(glm::mat3(transformMatrix))));
		}

		std::shared_ptr<Material> Mesh::getMaterial() const
		{
			return model.getMaterial(materialIndex);
		}

		const glm::vec3& Mesh::getPosition() const
		{
			se_assert(false); // TODO?
			return Primitive::getPosition();
		}
		const glm::vec3& Mesh::getScale() const
		{
			se_assert(false);
			return Primitive::getScale();
		}
		const glm::quat& Mesh::getRotation() const
		{
			se_assert(false);
			return Primitive::getRotation();
		}

		void Mesh::setPosition(const glm::vec3& _position)
		{
			se_assert(false);
		}
		void Mesh::setScale(const glm::vec3& _scale)
		{
			se_assert(false);
		}
		void Mesh::setRotation(const glm::quat& _rotation)
		{
			se_assert(false);
		}
	}
}
