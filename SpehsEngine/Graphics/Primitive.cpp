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
		}
		Primitive::~Primitive()
		{
			destroyedSignal();
		}

		const std::string& Primitive::getName() const
		{
			return name;
		}
		const bool Primitive::getRenderState() const
		{
			return renderState;
		}
		const Shader* Primitive::getShader() const
		{
			return shader;
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
			renderState = _state;
		}
		void Primitive::toggleRenderState()
		{
			renderState = !renderState;
		}
		void Primitive::setShader(const Shader* _shader)
		{
			if (shader == _shader)
				return;
			shader = _shader;
			enableBit(updateFlags, PrimitiveUpdateFlag::RenderInfoChanged);
		}
		void Primitive::setVertices(const VertexBuffer& _vertices)
		{
			vertices = _vertices;
			enableBit(updateFlags, PrimitiveUpdateFlag::VerticesChanged);
		}
		void Primitive::setIndices(const std::vector<uint16_t>& _indices)
		{
			indices = _indices;
			enableBit(updateFlags, PrimitiveUpdateFlag::IndicesChanged);
		}
		void Primitive::setColor(const Color& _color)
		{
			if (_color == primitiveColor)
				return;
			if (!checkBit(vertices.getAttributes(), VertexAttributeFlag::Color0))
			{
				log::warning("Cannot set primitive color, no color attribute in vertices!");
				return;
			}
			for (size_t i = 0; i < vertices.size(); i++)
				vertices.get<VertexAttributeFlag::Color0>(i) = _color;
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
