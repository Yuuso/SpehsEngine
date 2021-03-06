#pragma once

#include "SpehsEngine/Graphics/Primitive.h"


namespace se
{
	namespace graphics
	{
		class Model;
		class ModelNode;

		class Mesh : public Primitive
		{
		public:

			Mesh() = delete;
			Mesh(const Model& _model, const ModelNode& _node, const size_t _materialIndex, std::shared_ptr<VertexBuffer> _vertexBuffer, std::shared_ptr<IndexBuffer> _indexBuffer);

			std::shared_ptr<Material> getMaterial() const override;

			const glm::vec3& getPosition() const override;
			const glm::vec3& getScale() const override;
			const glm::quat& getRotation() const override;

			void setPosition(const glm::vec3& _position) override;
			void setScale(const glm::vec3& _scale) override;
			void setRotation(const glm::quat& _rotation) override;

		protected:

			void update() override;
			void updateMatrices() override;

		private:

			const Model& model;
			const ModelNode& node;

			const size_t materialIndex = 0;
		};
	}
}
