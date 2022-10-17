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
			Mesh(Model& _model, const ModelNode& _node, const size_t _materialIndex, std::shared_ptr<VertexBuffer> _vertexBuffer, std::shared_ptr<IndexBuffer> _indexBuffer);

			void addBone(const ModelNode* _boneNode, const glm::mat4& _offsetMatrix);
			virtual void updateMatrices() override;
			void clearOverriddenModelAttirbutes();

			virtual void								setRenderState(bool _state) override;
			virtual void								toggleRenderState() override;
			virtual void								setMaterial(std::shared_ptr<Material> _material) override;
			virtual void								setColor(const Color& _color) override;
			virtual void								setRenderFlags(RenderFlagsType _renderFlags) override;
			virtual void								enableRenderFlags(RenderFlagsType _renderFlags) override;
			virtual void								disableRenderFlags(RenderFlagsType _renderFlags) override;
			virtual void								setPrimitiveType(PrimitiveType _primitiveType) override;
			virtual void								setRenderMode(RenderMode _renderMode) override;
			virtual void								setInstances(std::shared_ptr<VertexBuffer> _instances) override;

			virtual bool								getRenderState() const override;
			virtual std::shared_ptr<const Material>		getMaterial() const override;
			virtual std::shared_ptr<Material>			getMaterial() override;
			virtual std::shared_ptr<const VertexBuffer>	getInstances() const override;
			virtual std::shared_ptr<VertexBuffer>		getInstances() override;
			virtual const Color&						getColor() const override;
			virtual RenderFlagsType						getRenderFlags() const override;
			virtual PrimitiveType						getPrimitiveType() const override;
			virtual RenderMode							getRenderMode() const override;
			virtual const UniformMatrices&				getTransformMatrices() const override;
			virtual const UniformMatrices&				getNormalMatrices() const override;

		protected:

			void update() override;

		private:

			Model& model; // Needs to be a mutable reference, because graphics systems needs to register as a renderer for the Model's buffers through the primitive
			const ModelNode& node;

			const size_t materialIndex = 0;
			std::vector<Bone> bones;

			UniformMatrices combinedTransformMatrices;
			UniformMatrices combinedNormalMatrices;

			PrimitiveAttributeFlagsType overriddenModelAttributes = 0;
		};
	}
}
