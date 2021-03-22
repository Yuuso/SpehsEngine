#include "stdafx.h"
#include "SpehsEngine/Graphics/Primitive.h"

#include "SpehsEngine/Core/BitwiseOperations.h"
#include "SpehsEngine/Graphics/Internal/InternalTypes.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/transform.hpp"


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
			if (vertices && vertices->getBufferChanged())
			{
				vertices->updateBuffer();
			}
			if (indices && indices->getBufferChanged())
			{
				indices->updateBuffer();
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
			transformMatrices[0] = glm::translate(getPosition()) * glm::mat4_cast(getRotation()) * glm::scale(getScale());
			normalMatrices[0] = glm::mat4(glm::inverse(glm::transpose(glm::mat3(transformMatrices[0]))));
		}
		bool Primitive::getVerticesChanged()
		{
			return checkBit(updateFlags, PrimitiveUpdateFlag::VerticesChanged) || (vertices ? vertices->getBufferChanged() : false);
		}
		bool Primitive::getIndicesChanged()
		{
			return checkBit(updateFlags, PrimitiveUpdateFlag::IndicesChanged) || (indices ? indices->getBufferChanged() : false);
		}

		const std::string& Primitive::getName() const
		{
			return name;
		}
		const bool Primitive::getRenderState() const
		{
			return renderState;
		}
		std::shared_ptr<Material> Primitive::getMaterial() const
		{
			return material;
		}
		std::shared_ptr<VertexBuffer> Primitive::getVertices() const
		{
			return vertices;
		}
		std::shared_ptr<IndexBuffer> Primitive::getIndices() const
		{
			return indices;
		}
		const Color& Primitive::getColor() const
		{
			return primitiveColor;
		}

		const RenderFlagsType Primitive::getRenderFlags() const
		{
			return renderFlags;
		}
		const bool Primitive::checkRenderFlags(const RenderFlagsType _renderFlags) const
		{
			return checkBit(renderFlags, _renderFlags);
		}
		const PrimitiveType Primitive::getPrimitiveType() const
		{
			return primitiveType;
		}
		const RenderMode Primitive::getRenderMode() const
		{
			return renderMode;
		}
		const RenderCopy* Primitive::getRenderCopy() const
		{
			return renderCopy.get();
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

		void Primitive::setName(const std::string_view _name)
		{
			name = _name;
		}
		void Primitive::setRenderState(const bool _state)
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
		void Primitive::setColor(const Color& _color)
		{
			primitiveColor = _color;
			enableBit(updateFlags, PrimitiveUpdateFlag::RenderInfoChanged);
		}

		void Primitive::setRenderFlags(const RenderFlagsType _renderFlags)
		{
			if (renderFlags == _renderFlags)
				return;
			renderFlags = _renderFlags;
			enableBit(updateFlags, PrimitiveUpdateFlag::RenderInfoChanged);
		}
		void Primitive::enableRenderFlags(const RenderFlagsType _renderFlags)
		{
			if (checkBit(renderFlags, _renderFlags))
				return;
			enableBit(renderFlags, _renderFlags);
			enableBit(updateFlags, PrimitiveUpdateFlag::RenderInfoChanged);
		}
		void Primitive::disableRenderFlags(const RenderFlagsType _renderFlags)
		{
			if (!checkBit(renderFlags, _renderFlags))
				return;
			disableBit(renderFlags, _renderFlags);
			enableBit(updateFlags, PrimitiveUpdateFlag::RenderInfoChanged);
		}
		void Primitive::setPrimitiveType(const PrimitiveType _primitiveType)
		{
			if (primitiveType == _primitiveType)
				return;
			primitiveType = _primitiveType;
			enableBit(updateFlags, PrimitiveUpdateFlag::RenderInfoChanged);
		}
		void Primitive::setRenderMode(const RenderMode _renderMode)
		{
			if (renderMode == _renderMode)
				return;
			renderMode = _renderMode;
			enableBit(updateFlags, PrimitiveUpdateFlag::RenderInfoChanged);
		}
		void Primitive::setRenderCopy(const RenderCopy& _renderCopy)
		{
			renderCopy = std::make_unique<RenderCopy>(_renderCopy);
		}
		void Primitive::removeRenderCopy()
		{
			renderCopy.reset();
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
	}
}
