#include "stdafx.h"
#include "SpehsEngine/Graphics/Primitive.h"

#include "SpehsEngine/Core/BitwiseOperations.h"
#include "SpehsEngine/Graphics/Internal/InternalTypes.h"
#include "SpehsEngine/Math/GLMMatrixUtilityFunctions.h"


namespace se
{
	namespace graphics
	{
		Primitive::Primitive()
		{
			transformMatrices.resize(1);
			normalMatrices.resize(1);
		}
		Primitive::~Primitive()
		{
			destroyedSignal();
		}

		void Primitive::update()
		{
			if (checkBit(updateFlags, PrimitiveUpdateFlag::TransformChanged))
			{
				updateMatrices();
			}
			if (auto _vertices = getVertices())
			{
				if (_vertices->getBufferChanged())
					vertices->updateBuffer();
			}
			if (auto _indices = getIndices())
			{
				if (_indices->getBufferChanged())
					_indices->updateBuffer();
			}
			if (auto _instances = getInstances())
			{
				if (_instances->getBufferChanged())
					_instances->updateBuffer();
			}
			// RenderInfoChanged changes only impact PrimitiveInternal
			updateFlags = 0;
		}
		const UniformMatrices& Primitive::getTransformMatrices() const
		{
			return transformMatrices;
		}
		const UniformMatrices& Primitive::getNormalMatrices() const
		{
			return normalMatrices;
		}
		void Primitive::updateMatrices()
		{
			transformMatrices[0] = constructTransformationMatrix(getPosition(), getRotation(), getScale());
			normalMatrices[0] = constructNormalMatrix(transformMatrices[0]);
		}
		void Primitive::removeFromScenes()
		{
			destroyedSignal();
		}
		bool Primitive::getVerticesChanged()
		{
			return checkBit(updateFlags, PrimitiveUpdateFlag::VerticesChanged) || (getVertices() ? getVertices()->getBufferChanged() : false);
		}
		bool Primitive::getIndicesChanged()
		{
			return checkBit(updateFlags, PrimitiveUpdateFlag::IndicesChanged) || (getIndices() ? getIndices()->getBufferChanged() : false);
		}

		const std::string& Primitive::getName() const
		{
			return name;
		}
		bool Primitive::getRenderState() const
		{
			return renderState;
		}
		std::shared_ptr<const Material>	Primitive::getMaterial() const
		{
			return material;
		}
		std::shared_ptr<Material> Primitive::getMaterial()
		{
			return material;
		}
		std::shared_ptr<const VertexBuffer>	Primitive::getVertices() const
		{
			return vertices;
		}
		std::shared_ptr<VertexBuffer> Primitive::getVertices()
		{
			return vertices;
		}
		std::shared_ptr<const IndexBuffer> Primitive::getIndices() const
		{
			return indices;
		}
		std::shared_ptr<IndexBuffer> Primitive::getIndices()
		{
			return indices;
		}
		std::shared_ptr<const VertexBuffer> Primitive::getInstances() const
		{
			return instances;
		}
		std::shared_ptr<VertexBuffer> Primitive::getInstances()
		{
			return instances;
		}
		const Color& Primitive::getColor() const
		{
			return primitiveColor;
		}
		const Scissor& Primitive::getScissor() const
		{
			return scissor;
		}

		RenderFlagsType Primitive::getRenderFlags() const
		{
			return renderFlags;
		}
		bool Primitive::checkRenderFlags(RenderFlagsType _renderFlags) const
		{
			return checkBit(getRenderFlags(), _renderFlags);
		}
		PrimitiveType Primitive::getPrimitiveType() const
		{
			return primitiveType;
		}
		RenderMode Primitive::getRenderMode() const
		{
			return renderMode;
		}
		uint32_t Primitive::getRenderSortDepth() const
		{
			switch (renderSortDepth)
			{
				case RenderSortDepth::Default:		return renderDepthValue;
				case RenderSortDepth::ZPosition:	return (uint32_t)(getPosition().z + (float)(std::numeric_limits<int16_t>::max()));
			}
			se_assert(false);
			return 0u;
		}
		const RenderCopy* Primitive::getRenderCopy() const
		{
			return renderCopy.get();
		}
		const Stencil* Primitive::getStencil() const
		{
			return stencil.get();
		}

		const glm::vec3& Primitive::getPosition() const
		{
			return position;
		}
		const glm::vec3& Primitive::getScale() const
		{
			return scale;
		}
		const glm::quat& Primitive::getRotation() const
		{
			return rotation;
		}

		void Primitive::setName(std::string_view _name)
		{
			name = _name;
		}
		void Primitive::setRenderState(bool _state)
		{
			if (renderState == _state)
				return;
			renderState = _state;

			// Update everything when re-enabling rendering
			if (renderState)
				enableBit(updateFlags, PrimitiveUpdateFlag::EverythingChanged);
		}
		void Primitive::toggleRenderState()
		{
			setRenderState(!renderState);
		}
		void Primitive::setMaterial(std::shared_ptr<Material> _material)
		{
			if (material == _material)
				return;
			material = _material;
			enableBit(updateFlags, PrimitiveUpdateFlag::RenderInfoChanged);
		}
		void Primitive::setVertices(std::shared_ptr<VertexBuffer> _vertices)
		{
			vertices = _vertices;
			enableBit(updateFlags, PrimitiveUpdateFlag::VerticesChanged);
		}
		void Primitive::setIndices(std::shared_ptr<IndexBuffer> _indices)
		{
			indices = _indices;
			enableBit(updateFlags, PrimitiveUpdateFlag::IndicesChanged);
		}
		void Primitive::setInstances(std::shared_ptr<VertexBuffer> _instances)
		{
			instances = _instances;
			validateRenderMode();
		}
		void Primitive::setColor(const Color& _color)
		{
			primitiveColor = _color;
			enableBit(updateFlags, PrimitiveUpdateFlag::RenderInfoChanged);
		}
		void Primitive::setScissor(const Scissor& _scissor)
		{
			if (scissor == _scissor)
				return;
			scissor = _scissor;
			validateRenderMode();
		}

		void Primitive::setRenderFlags(RenderFlagsType _renderFlags)
		{
			if (renderFlags == _renderFlags)
				return;
			renderFlags = _renderFlags;
			enableBit(updateFlags, PrimitiveUpdateFlag::RenderInfoChanged);
		}
		void Primitive::enableRenderFlags(RenderFlagsType _renderFlags)
		{
			if (checkBit(renderFlags, _renderFlags))
				return;
			enableBit(renderFlags, _renderFlags);
			enableBit(updateFlags, PrimitiveUpdateFlag::RenderInfoChanged);
		}
		void Primitive::disableRenderFlags(RenderFlagsType _renderFlags)
		{
			if (!checkBit(renderFlags, _renderFlags))
				return;
			disableBit(renderFlags, _renderFlags);
			enableBit(updateFlags, PrimitiveUpdateFlag::RenderInfoChanged);
		}
		void Primitive::setPrimitiveType(PrimitiveType _primitiveType)
		{
			if (primitiveType == _primitiveType)
				return;
			primitiveType = _primitiveType;
			enableBit(updateFlags, PrimitiveUpdateFlag::RenderInfoChanged);
		}
		void Primitive::setRenderMode(RenderMode _renderMode)
		{
			if (renderMode == _renderMode)
				return;
			renderMode = _renderMode;
			validateRenderMode();
			enableBit(updateFlags, PrimitiveUpdateFlag::RenderInfoChanged);
		}
		void Primitive::setRenderSortDepth(RenderSortDepth _renderDepth)
		{
			renderSortDepth = _renderDepth;
		}
		void Primitive::setRenderSortDepth(uint32_t _renderDepth)
		{
			renderSortDepth = RenderSortDepth::Default;
			renderDepthValue = _renderDepth;
			validateRenderMode();
		}
		void Primitive::setRenderCopy(const RenderCopy& _renderCopy)
		{
			renderCopy = std::make_unique<RenderCopy>(_renderCopy);
			validateRenderMode();
		}
		void Primitive::removeRenderCopy()
		{
			renderCopy.reset();
		}
		void Primitive::setStencil(const Stencil& _stencil)
		{
			stencil = std::make_unique<Stencil>(_stencil);
			validateRenderMode();
		}
		void Primitive::removeStencil()
		{
			stencil.reset();
		}

		void Primitive::setPosition(const glm::vec3& _position)
		{
			if (position == _position)
				return;
			position = _position;
			enableBit(updateFlags, PrimitiveUpdateFlag::TransformChanged);
		}
		void Primitive::setScale(const glm::vec3& _scale)
		{
			if (scale == _scale)
				return;
			scale = _scale;
			enableBit(updateFlags, PrimitiveUpdateFlag::TransformChanged);
		}
		void Primitive::setRotation(const glm::quat& _rotation)
		{
			if (rotation == _rotation)
				return;
			rotation = _rotation;
			enableBit(updateFlags, PrimitiveUpdateFlag::TransformChanged);
		}

		void Primitive::validateRenderMode()
		{
			if (renderMode == RenderMode::Static)
			{
				if (getInstances())
				{
					log::error("RenderMode::Static and instancing don't work together!");
				}
				if (getRenderCopy())
				{
					log::error("RenderMode::Static and RenderCopy don't work together!");
				}
				if (getScissor().enabled)
				{
					log::error("RenderMode::Static and Scissor don't work together!");
				}
				if (getStencil())
				{
					log::error("RenderMode::Static and Stencil don't work together!");
				}
				if (getRenderSortDepth() != 0 || renderSortDepth != RenderSortDepth::Default)
				{
					log::error("RenderMode::Static and RenderSortDepth don't work together!");
				}
			}
		}
	}
}
