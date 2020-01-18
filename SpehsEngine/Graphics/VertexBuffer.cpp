#include "stdafx.h"
#include "SpehsEngine/Graphics/VertexBuffer.h"


namespace se
{
	namespace graphics
	{
		VertexBuffer::VertexBuffer()
		{
		}
		VertexBuffer::VertexBuffer(const VertexAttributeFlagsType _vertexAttributes)
		{
			setAttributes(_vertexAttributes);
		}

		const VertexAttributeFlagsType VertexBuffer::getAttributes() const
		{
			return attributes;
		}
		void VertexBuffer::setAttributes(const VertexAttributeFlagsType _vertexAttributes)
		{
			se_assert(buffer.size() == 0); // TODO: re-order the buffer?!
			attributes = _vertexAttributes;
			calculateVertexSize();
		}

		const size_t VertexBuffer::size() const
		{
			return buffer.size() / vertexBytes;
		}
		const size_t VertexBuffer::bytes() const
		{
			return buffer.size();
		}
		void VertexBuffer::resize(const size_t _size)
		{
			buffer.resize(_size * vertexBytes);
		}
		void VertexBuffer::pushBack(const VertexBuffer& _vertices)
		{
			buffer.insert(buffer.end(), _vertices.buffer.begin(), _vertices.buffer.end());
		}
		void VertexBuffer::erase(const size_t _begin, const size_t _end)
		{
			buffer.erase(buffer.begin() + _begin * vertexBytes, buffer.begin() + _end * vertexBytes);
		}
		void VertexBuffer::clear()
		{
			buffer.clear();
		}
		const void* VertexBuffer::data() const
		{
			return (const void*)buffer.data();
		}

		void VertexBuffer::calculateVertexSize()
		{
			vertexBytes = 0;
			if(checkBit(attributes, Position))	{ vertexBytes += sizeof(glm::vec3); }
			if(checkBit(attributes, Normal))	{ vertexBytes += sizeof(glm::vec3); }
			if(checkBit(attributes, Tangent))	{ vertexBytes += sizeof(glm::vec3); }
			if(checkBit(attributes, Bitangent))	{ vertexBytes += sizeof(glm::vec3); }
			if(checkBit(attributes, Color0))	{ vertexBytes += sizeof(Color)	 ; }
			if(checkBit(attributes, Color1))	{ vertexBytes += sizeof(Color)	 ; }
			if(checkBit(attributes, Color2))	{ vertexBytes += sizeof(Color)	 ; }
			if(checkBit(attributes, Color3))	{ vertexBytes += sizeof(Color)	 ; }
			if(checkBit(attributes, Weight))	{ vertexBytes += sizeof(float)	 ; }
			if(checkBit(attributes, TexCoord0))	{ vertexBytes += sizeof(glm::vec2); }
			if(checkBit(attributes, TexCoord1))	{ vertexBytes += sizeof(glm::vec2); }
			if(checkBit(attributes, TexCoord2))	{ vertexBytes += sizeof(glm::vec2); }
			if(checkBit(attributes, TexCoord3))	{ vertexBytes += sizeof(glm::vec2); }
			if(checkBit(attributes, TexCoord4))	{ vertexBytes += sizeof(glm::vec2); }
			if(checkBit(attributes, TexCoord5))	{ vertexBytes += sizeof(glm::vec2); }
			if(checkBit(attributes, TexCoord6))	{ vertexBytes += sizeof(glm::vec2); }
			if(checkBit(attributes, TexCoord7))	{ vertexBytes += sizeof(glm::vec2); }
		}
	}
}