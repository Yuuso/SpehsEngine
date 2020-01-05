#include "stdafx.h"
#include "SpehsEngine/Graphics/Primitive.h"

#include "SpehsEngine/Core/BitwiseOperations.h"


namespace se
{
	namespace graphics
	{
		Primitive::Primitive()
		{
		}
		Primitive::~Primitive()
		{
		}

		const std::string& Primitive::nameGet() const
		{
			return name;
		}
		const bool Primitive::renderStateGet() const
		{
			return renderState;
		}
		const Shader* Primitive::shaderGet() const
		{
			return shader;
		}
		const std::vector<Vertex>& Primitive::verticesGet() const
		{
			return vertices;
		}
		const std::vector<IndexType>& Primitive::indicesGet() const
		{
			return indices;
		}

		const RenderFlagsType Primitive::renderFlagsGet() const
		{
			return renderFlags;
		}
		const bool Primitive::renderFlagCheck(const RenderFlag _renderFlag) const
		{
			return checkBit(renderFlags, _renderFlag);
		}
		const RenderStyle Primitive::renderStyleGet() const
		{
			return renderStyle;
		}
		const RenderMode Primitive::renderModeGet() const
		{
			return renderMode;
		}

		const glm::vec3& Primitive::positionGet() const
		{
			return position;
		}
		const glm::vec3& Primitive::localPositionGet() const
		{
			return localPosition;
		}
		const glm::vec3& Primitive::scaleGet() const
		{
			return scale;
		}
		const glm::quat& Primitive::rotationGet() const
		{
			return rotation;
		}

		void Primitive::nameSet(const std::string_view _name)
		{
			name = _name;
		}
		void Primitive::renderStateSet(const bool _state)
		{
			renderState = _state;
		}
		void Primitive::shaderSet(const Shader* _shader)
		{
			shader = _shader;
			enableBit(updateFlags, UpdateFlag::RenderInfoChanged);
		}
		void Primitive::verticesSet(const std::vector<Vertex>& _vertices)
		{
			vertices = _vertices;
			enableBit(updateFlags, UpdateFlag::VerticesChanged);
		}
		void Primitive::indicesSet(const std::vector<uint16_t>& _indices)
		{
			indices = _indices;
			enableBit(updateFlags, UpdateFlag::IndicesChanged);
		}

		void Primitive::renderFlagsSet(const RenderFlagsType _renderFlags)
		{
			renderFlags = _renderFlags;
			enableBit(updateFlags, UpdateFlag::RenderInfoChanged);
		}
		void Primitive::renderFlagEnable(const RenderFlag _renderFlag)
		{
			enableBit(renderFlags, _renderFlag);
			enableBit(updateFlags, UpdateFlag::RenderInfoChanged);
		}
		void Primitive::renderFlagDisable(const RenderFlag _renderFlag)
		{
			disableBit(renderFlags, _renderFlag);
			enableBit(updateFlags, UpdateFlag::RenderInfoChanged);
		}
		void Primitive::renderStyleSet(const RenderStyle _renderStyle)
		{
			renderStyle = _renderStyle;
			enableBit(updateFlags, UpdateFlag::RenderInfoChanged);
		}
		void Primitive::renderModeSet(const RenderMode _renderMode)
		{
			renderMode = _renderMode;
			enableBit(updateFlags, UpdateFlag::RenderInfoChanged);
		}

		void Primitive::positionSet(const glm::vec3& _position)
		{
			position = _position;
			enableBit(updateFlags, UpdateFlag::TransformChanged);
		}
		void Primitive::localPositionSet(const glm::vec3& _position)
		{
			localPosition = _position;
			enableBit(updateFlags, UpdateFlag::TransformChanged);
		}
		void Primitive::scaleSet(const glm::vec3& _scale)
		{
			scale = _scale;
			enableBit(updateFlags, UpdateFlag::TransformChanged);
		}
		void Primitive::rotationSet(const glm::quat& _rotation)
		{
			rotation = _rotation;
			enableBit(updateFlags, UpdateFlag::TransformChanged);
		}
	}
}
