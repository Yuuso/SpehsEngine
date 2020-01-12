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
		const std::vector<Vertex>& Primitive::getVertices() const
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
			shader = _shader;
			enableBit(updateFlags, UpdateFlag::RenderInfoChanged);
		}
		void Primitive::setVertices(const std::vector<Vertex>& _vertices)
		{
			vertices = _vertices;
			enableBit(updateFlags, UpdateFlag::VerticesChanged);
		}
		void Primitive::setIndices(const std::vector<uint16_t>& _indices)
		{
			indices = _indices;
			enableBit(updateFlags, UpdateFlag::IndicesChanged);
		}

		void Primitive::setRenderFlags(const RenderFlagsType _renderFlags)
		{
			renderFlags = _renderFlags;
			enableBit(updateFlags, UpdateFlag::RenderInfoChanged);
		}
		void Primitive::enableRenderFlag(const RenderFlag _renderFlag)
		{
			enableBit(renderFlags, _renderFlag);
			enableBit(updateFlags, UpdateFlag::RenderInfoChanged);
		}
		void Primitive::disableRenderFlag(const RenderFlag _renderFlag)
		{
			disableBit(renderFlags, _renderFlag);
			enableBit(updateFlags, UpdateFlag::RenderInfoChanged);
		}
		void Primitive::setPrimitiveType(const PrimitiveType _primitiveType)
		{
			primitiveType = _primitiveType;
			enableBit(updateFlags, UpdateFlag::RenderInfoChanged);
		}
		void Primitive::setRenderMode(const RenderMode _renderMode)
		{
			renderMode = _renderMode;
			enableBit(updateFlags, UpdateFlag::RenderInfoChanged);
		}

		void Primitive::setPosition(const glm::vec3& _position)
		{
			position = _position;
			enableBit(updateFlags, UpdateFlag::TransformChanged);
		}
		void Primitive::setLocalPosition(const glm::vec3& _position)
		{
			localPosition = _position;
			enableBit(updateFlags, UpdateFlag::TransformChanged);
		}
		void Primitive::setScale(const glm::vec3& _scale)
		{
			scale = _scale;
			enableBit(updateFlags, UpdateFlag::TransformChanged);
		}
		void Primitive::setLocalScale(const glm::vec3& _scale)
		{
			localScale = _scale;
			enableBit(updateFlags, UpdateFlag::TransformChanged);
		}
		void Primitive::setRotation(const glm::quat& _rotation)
		{
			rotation = _rotation;
			enableBit(updateFlags, UpdateFlag::TransformChanged);
		}
		void Primitive::setLocalRotation(const glm::quat& _rotation)
		{
			localRotation = _rotation;
			enableBit(updateFlags, UpdateFlag::TransformChanged);
		}
	}
}
