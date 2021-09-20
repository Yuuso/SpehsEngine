#pragma once

#include "SpehsEngine/Graphics/Internal/Bone.h"
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

			void addBone(const ModelNode* _boneNode, const glm::mat4& _offsetMatrix);

			void updateMatrices() override;
			const UniformMatrices& getTransformMatrices() const override;
			const UniformMatrices& getNormalMatrices() const override;

		protected:

			void update() override;

		private:

			const Model& model;
			const ModelNode& node;

			const size_t materialIndex = 0;
			std::vector<Bone> bones;

			UniformMatrices combinedTransformMatrices;
			UniformMatrices combinedNormalMatrices;
		};
	}
}
