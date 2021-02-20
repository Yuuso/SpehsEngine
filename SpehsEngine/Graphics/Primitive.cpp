#include "stdafx.h"
#include "SpehsEngine/Graphics/Primitive.h"

#include "SpehsEngine/Core/BitwiseOperations.h"
#include "SpehsEngine/Graphics/Internal/InternalTypes.h"


namespace se
{
	namespace graphics
	{
		Primitive::Primitive()
		{
			invalidatePrimitiveColors();
		}
		Primitive::~Primitive()
		{
			destroyedSignal();
		}
		void Primitive::invalidatePrimitiveColors()
		{
			for (size_t i = 0; i < MAX_PRIMITIVE_COLORS; i++)
			{
				primitiveColor[i] = se::Color(-1.0f, -1.0f, -1.0f, -1.0f);
			}
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
		const VertexBuffer& Primitive::getVertices() const
		{
			return vertices;
		}
		const std::vector<IndexType>& Primitive::getIndices() const
		{
			return indices;
		}

		const RenderFlagsType Primitive::getRenderFlags() const
		{
			return renderFlags;
		}
		const bool Primitive::checkRenderFlag(const RenderFlag _renderFlag) const
		{
			return checkBit(renderFlags, _renderFlag);
		}
		const PrimitiveType Primitive::getPrimitiveType() const
		{
			return primitiveType;
		}
		const RenderMode Primitive::getRenderMode() const
		{
			return renderMode;
		}

		const glm::vec3& Primitive::getPosition() const
		{
			return position;
		}
		const glm::vec3& Primitive::getLocalPosition() const
		{
			return localPosition;
		}
		const glm::vec3& Primitive::getScale() const
		{
			return scale;
		}
		const glm::vec3& Primitive::getLocalScale() const
		{
			return localScale;
		}
		const glm::quat& Primitive::getRotation() const
		{
			return rotation;
		}
		const glm::quat& Primitive::getLocalRotation() const
		{
			return localRotation;
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
		void Primitive::setVertices(const VertexBuffer& _vertices)
		{
			vertices = _vertices;
			invalidatePrimitiveColors();
			enableBit(updateFlags, PrimitiveUpdateFlag::VerticesChanged);
		}
		void Primitive::setIndices(const std::vector<IndexType>& _indices)
		{
			indices = _indices;
			enableBit(updateFlags, PrimitiveUpdateFlag::IndicesChanged);
		}
		void Primitive::setColor(const Color& _color, const size_t _colorIndex)
		{
			if (_colorIndex >= MAX_PRIMITIVE_COLORS)
			{
				log::warning("Invalid primitive color index: " + std::to_string(_colorIndex));
				return;
			}
			if (_color == primitiveColor[_colorIndex])
				return;

			VertexAttributeFlagsType attribute = 0;
			switch (_colorIndex)
			{
				case 0: enableBit(attribute, VertexAttribute::Color0); break;
				case 1: enableBit(attribute, VertexAttribute::Color1); break;
				case 2: enableBit(attribute, VertexAttribute::Color2); break;
				case 3: enableBit(attribute, VertexAttribute::Color3); break;
			}
			if (!checkBit(vertices.getAttributes(), attribute))
			{
				log::warning("Cannot set primitive color, no color " + std::to_string(_colorIndex) + " attribute in vertex buffer!");
				return;
			}
			primitiveColor[_colorIndex] = _color;
			for (size_t i = 0; i < vertices.size(); i++)
				vertices.get<VertexAttribute::Color0>(i) = _color;
			enableBit(updateFlags, PrimitiveUpdateFlag::VerticesChanged);
		}

		void Primitive::setRenderFlags(const RenderFlagsType _renderFlags)
		{
			if (renderFlags == _renderFlags)
				return;
			renderFlags = _renderFlags;
			enableBit(updateFlags, PrimitiveUpdateFlag::RenderInfoChanged);
		}
		void Primitive::enableRenderFlag(const RenderFlag _renderFlag)
		{
			if (checkBit(renderFlags, _renderFlag))
				return;
			enableBit(renderFlags, _renderFlag);
			enableBit(updateFlags, PrimitiveUpdateFlag::RenderInfoChanged);
		}
		void Primitive::disableRenderFlag(const RenderFlag _renderFlag)
		{
			if (!checkBit(renderFlags, _renderFlag))
				return;
			disableBit(renderFlags, _renderFlag);
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

		void Primitive::setPosition(const glm::vec3& _position)
		{
			if (position == _position)
				return;
			position = _position;
			enableBit(updateFlags, PrimitiveUpdateFlag::TransformChanged);
		}
		void Primitive::setLocalPosition(const glm::vec3& _position)
		{
			if (localPosition == _position)
				return;
			localPosition = _position;
			enableBit(updateFlags, PrimitiveUpdateFlag::TransformChanged);
		}
		void Primitive::setScale(const glm::vec3& _scale)
		{
			if (scale == _scale)
				return;
			scale = _scale;
			enableBit(updateFlags, PrimitiveUpdateFlag::TransformChanged);
		}
		void Primitive::setLocalScale(const glm::vec3& _scale)
		{
			if (localScale == _scale)
				return;
			localScale = _scale;
			enableBit(updateFlags, PrimitiveUpdateFlag::TransformChanged);
		}
		void Primitive::setRotation(const glm::quat& _rotation)
		{
			if (rotation == _rotation)
				return;
			rotation = _rotation;
			enableBit(updateFlags, PrimitiveUpdateFlag::TransformChanged);
		}
		void Primitive::setLocalRotation(const glm::quat& _rotation)
		{
			if (localRotation == _rotation)
				return;
			localRotation = _rotation;
			enableBit(updateFlags, PrimitiveUpdateFlag::TransformChanged);
		}
	}
}
