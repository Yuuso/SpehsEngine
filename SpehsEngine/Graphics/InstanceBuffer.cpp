#include "stdafx.h"
#include "SpehsEngine/Graphics/InstanceBuffer.h"

#include "bgfx/bgfx.h"
#include "SpehsEngine/Graphics/Internal/InternalUtilities.h"
#include "SpehsEngine/Math/GLMMatrixUtilityFunctions.h"


namespace se
{
	namespace graphics
	{
		static constexpr VertexAttributeFlagsType instanceDataAttributes =
			  VertexAttribute::Data0
			| VertexAttribute::Data1
			| VertexAttribute::Data2
			| VertexAttribute::Data3;

		static_assert(
			  sizeof(VertexAttribute::Data0Type)
			+ sizeof(VertexAttribute::Data1Type)
			+ sizeof(VertexAttribute::Data2Type)
			+ sizeof(VertexAttribute::Data3Type)
			== sizeof(glm::mat4), "Invalid vertex attribute data types for instancing!");

		InstanceBuffer::InstanceBuffer()
		{
		}
		InstanceBuffer::~InstanceBuffer()
		{
			safeDestroy<bgfx::DynamicVertexBufferHandle>(bufferObject);
		}

		InstanceBuffer::InstanceBuffer(const InstanceBuffer& _other)
			: BufferObject(_other)
			, buffer(_other.buffer)
		{
		}
		InstanceBuffer& InstanceBuffer::operator=(const InstanceBuffer& _other)
		{
			BufferObject::operator=(_other);
			buffer = _other.buffer;
			return *this;
		}

		InstanceBuffer::InstanceBuffer(InstanceBuffer&& _other)
			: BufferObject(std::move(_other))
			, buffer(std::move(_other.buffer))
		{
		}
		InstanceBuffer& InstanceBuffer::operator=(InstanceBuffer&& _other)
		{
			BufferObject::operator=(std::move(_other));
			buffer = std::move(_other.buffer);
			return *this;
		}


		const size_t InstanceBuffer::size() const
		{
			return buffer.size();
		}
		const size_t InstanceBuffer::bytes() const
		{
			return buffer.size() * sizeof(glm::mat4);
		}
		void InstanceBuffer::resize(const size_t _size)
		{
			buffer.resize(_size);
			bufferChanged = true;
		}
		void InstanceBuffer::grow(const size_t _amount)
		{
			buffer.resize(buffer.size() + _amount);
			bufferChanged = true;
		}
		void InstanceBuffer::pushBack(const InstanceData& _data)
		{
			const size_t index = buffer.size();
			grow(1);
			set(index, _data);
			bufferChanged = true;
		}
		void InstanceBuffer::erase(const size_t _begin, const size_t _end)
		{
			buffer.erase(buffer.begin() + _begin, buffer.begin() + _end);
			bufferChanged = true;
		}
		void InstanceBuffer::clear()
		{
			buffer.clear();
			bufferChanged = true;
		}
		const void* InstanceBuffer::data() const
		{
			return reinterpret_cast<const void*>(buffer.data());
		}

		void InstanceBuffer::set(const size_t _at, const InstanceData& _data)
		{
			buffer[_at] = constructTransformationMatrix(_data.position, _data.rotation, glm::vec3(_data.scale));
			bufferChanged = true;
		}
		const InstanceData InstanceBuffer::get(const size_t _at) const
		{
			InstanceData result;
			glm::vec3 temp;
			decomposeTransformationMatrix(buffer[_at], result.position, result.rotation, temp);
			se_assert(temp.x == temp.y && temp.y == temp.z);
			result.scale = temp.x;
			return result;
		}

		void InstanceBuffer::updateBuffer()
		{
			if (renderers.size() > 0 && size() > 0)
			{
				const bgfx::Memory* bufferMemory = bgfx::copy(data(), uint32_t(bytes()));
				if (bufferObject == INVALID_RESOURCE_HANDLE)
				{
					bufferObject = bgfx::createDynamicVertexBuffer(bufferMemory, findVertexLayout(instanceDataAttributes)).idx;
				}
				else
				{
					bgfx::DynamicVertexBufferHandle dvbh = { bufferObject };
					bgfx::update(dvbh, 0, bufferMemory);
				}
			}
			else
			{
				safeDestroy<bgfx::DynamicVertexBufferHandle>(bufferObject);
			}
			bufferChanged = false;
		}
	}
}
