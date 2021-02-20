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
		void VertexBuffer::grow(const size_t _amount)
		{
			buffer.resize(buffer.size() + _amount * vertexBytes);
		}
		void VertexBuffer::pushBack(const VertexBuffer& _vertices)
		{
			se_assert(getAttributes() == _vertices.getAttributes());
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

			#define check_attribute_bytes(__attribute) \
				if(checkBit(attributes, VertexAttribute::VertexAttributeFlag::__attribute)) { vertexBytes += sizeof(VertexAttribute::##__attribute##Type); }

			check_attribute_bytes(Position)
			check_attribute_bytes(Normal)
			check_attribute_bytes(Tangent)
			check_attribute_bytes(Bitangent)
			check_attribute_bytes(Color0)
			check_attribute_bytes(Color1)
			check_attribute_bytes(Color2)
			check_attribute_bytes(Color3)
			check_attribute_bytes(Weight)
			check_attribute_bytes(Indices)
			check_attribute_bytes(TexCoord0)
			check_attribute_bytes(TexCoord1)
			check_attribute_bytes(TexCoord2)
			check_attribute_bytes(Data0)
			check_attribute_bytes(Data1)
			check_attribute_bytes(Data2)
			check_attribute_bytes(Data3)
			check_attribute_bytes(Data4)

			#undef check_attribute_bytes
		}
	}
}