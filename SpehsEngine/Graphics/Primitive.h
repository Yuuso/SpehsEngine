#pragma once


namespace se
{
	namespace graphics
	{
		class IndexBuffer;
		class Material;
		struct RenderCopy;
		class VertexBuffer;

		class Primitive
		{
		public:

														Primitive();
			virtual										~Primitive();

														Primitive(const Primitive& _other) = delete;
			Primitive&									operator=(const Primitive& _other) = delete;

														Primitive(Primitive&& _other) = delete;
			Primitive&									operator=(Primitive&& _other) = delete;


			virtual void								updateMatrices();
			virtual void								removeFromScenes();

			virtual const std::string&					getName() const;
			virtual bool								getRenderState() const;
			virtual std::shared_ptr<const Material>		getMaterial() const;
			virtual std::shared_ptr<Material>			getMaterial();
			virtual std::shared_ptr<const VertexBuffer>	getVertices() const;
			virtual std::shared_ptr<VertexBuffer>		getVertices();
			virtual std::shared_ptr<const IndexBuffer>	getIndices() const;
			virtual std::shared_ptr<IndexBuffer>		getIndices();
			virtual std::shared_ptr<const VertexBuffer>	getInstances() const;
			virtual std::shared_ptr<VertexBuffer>		getInstances();
			virtual const Color&						getColor() const;
			virtual const Scissor&						getScissor() const;
			virtual RenderFlagsType						getRenderFlags() const;
			virtual bool								checkRenderFlags(RenderFlagsType _renderFlags) const;
			virtual PrimitiveType						getPrimitiveType() const;
			virtual RenderMode							getRenderMode() const;
			virtual uint32_t							getRenderSortDepth() const;
			virtual const RenderCopy*					getRenderCopy() const;
			virtual const Stencil*						getStencil() const;
			virtual const glm::vec3&					getPosition() const;
			virtual const glm::vec3&					getScale() const;
			virtual const glm::quat&					getRotation() const;
			virtual const UniformMatrices&				getTransformMatrices() const;
			virtual const UniformMatrices&				getNormalMatrices() const;

			virtual void								setName(std::string_view _name);
			virtual void								setRenderState(bool _state);
			virtual void								toggleRenderState();
			virtual void								setMaterial(std::shared_ptr<Material> _material);
			virtual void								setVertices(std::shared_ptr<VertexBuffer> _vertices);
			virtual void								setIndices(std::shared_ptr<IndexBuffer> _indices);
			virtual void								setInstances(std::shared_ptr<VertexBuffer> _instances);
			virtual void								setColor(const Color& _color);
			virtual void								setScissor(const Scissor& _scissor);
			virtual void								setRenderFlags(RenderFlagsType _renderFlags);
			virtual void								enableRenderFlags(RenderFlagsType _renderFlags);
			virtual void								disableRenderFlags(RenderFlagsType _renderFlags);
			virtual void								setPrimitiveType(PrimitiveType _primitiveType);
			virtual void								setRenderMode(RenderMode _renderMode);
			virtual void								setRenderSortDepth(RenderSortDepth _renderDepth);
			virtual void								setRenderSortDepth(uint32_t _renderDepth);
			virtual void								setRenderCopy(const RenderCopy& _renderCopy);
			virtual void								removeRenderCopy();
			virtual void								setStencil(const Stencil& _stencil);
			virtual void								removeStencil();
			virtual void								setPosition(const glm::vec3& _position);
			virtual void								setScale(const glm::vec3& _scale);
			virtual void								setRotation(const glm::quat& _rotation);

		protected:

			virtual void								update();
			bool										getVerticesChanged();
			bool										getIndicesChanged();

			PrimitiveUpdateFlagsType					updateFlags = 0;

		private:

			friend class PrimitiveInternal;
			friend class Model;

			void										validateRenderMode();

			std::string									name					= "primitive";
			bool										renderState				= true;
			RenderFlagsType								renderFlags				= defaultRenderFlags;
			PrimitiveType								primitiveType			= defaultPrimitiveType;
			RenderMode									renderMode				= defaultRenderMode;
			RenderSortDepth								renderSortDepth			= RenderSortDepth::Default;
			uint32_t									renderDepthValue		= 0;
			std::unique_ptr<RenderCopy>					renderCopy;
			std::unique_ptr<Stencil>					stencil;
			std::shared_ptr<Material>					material				= nullptr;
			std::shared_ptr<VertexBuffer>				vertices;
			std::shared_ptr<IndexBuffer>				indices;
			std::shared_ptr<VertexBuffer>				instances;
			glm::vec3									position				= glm::vec3(0.0f);
			glm::vec3									scale					= glm::vec3(1.0f);
			glm::quat									rotation				= glm::vec3(0.0f);
			Color										primitiveColor;
			Scissor										scissor;

			UniformMatrices								transformMatrices;
			UniformMatrices								normalMatrices;

			boost::signals2::signal<void(void)>			destroyedSignal;
		};
	}
}
