#include "stdafx.h"
#include "SpehsEngine/Graphics/IndexBuffer.h"


namespace se
{
	namespace graphics
	{
		IndexBuffer::~IndexBuffer()
		{
			safeDestroy<bgfx::IndexBufferHandle>(bufferObject);
		}

		IndexBuffer::IndexBuffer(const IndexBuffer& _other)
			: BufferObject(_other)
			, buffer(_other.buffer)
		{}
		IndexBuffer& IndexBuffer::operator=(const IndexBuffer& _other)
		{
			BufferObject::operator=(_other);
			buffer = _other.buffer;
			return *this;
		}

		IndexBuffer::IndexBuffer(IndexBuffer&& _other)
			: BufferObject(std::move(_other))
			, buffer(std::move(_other.buffer))
		{}
		IndexBuffer& IndexBuffer::operator=(IndexBuffer&& _other)
		{
			BufferObject::operator=(std::move(_other));
			buffer = _other.buffer;
			return *this;
		}

		void IndexBuffer::updateBuffer()
		{
			safeDestroy<bgfx::IndexBufferHandle>(bufferObject);
			if (renderers.size() > 0 && size() > 0)
			{
				static_assert(sizeof(IndexType) == 2);
				const bgfx::Memory* bufferMemory = bgfx::copy(data(), uint32_t(bytes()));
				bufferObject = bgfx::createIndexBuffer(bufferMemory).idx;
				if (bufferObject == INVALID_RESOURCE_HANDLE)
					log::fatal("Out of index buffer memory?");
			}
			bufferChanged = false;
		}
	}
}
