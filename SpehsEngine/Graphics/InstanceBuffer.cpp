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
			setChangedAll();
			bufferChanged = true;
		}
		void InstanceBuffer::grow(const size_t _amount)
		{
			buffer.resize(buffer.size() + _amount);
			setChangedAll();
			bufferChanged = true;
		}
		void InstanceBuffer::pushBack(const InstanceData& _data)
		{
			const size_t index = buffer.size();
			grow(1);
			set(index, _data);
			setChangedAll();
			bufferChanged = true;
		}
		void InstanceBuffer::erase(const size_t _begin, const size_t _end)
		{
			buffer.erase(buffer.begin() + _begin, buffer.begin() + _end);
			setChangedAll();
			bufferChanged = true;
		}
		void InstanceBuffer::clear()
		{
			buffer.clear();
			setChangedAll();
			bufferChanged = true;
		}
		const void* InstanceBuffer::data(const size_t _index) const
		{
			return reinterpret_cast<const void*>(&buffer[_index]);
		}

		void InstanceBuffer::set(const size_t _at, const InstanceData& _data)
		{
			se_assert(_at < size());
			buffer[_at] = constructTransformationMatrix(_data.position, _data.rotation, glm::vec3(_data.scale));
			changedBegin = std::min(changedBegin, _at);
			changedBegin = std::max(changedEnd, _at + 1);
			bufferChanged = true;
		}
		const InstanceData InstanceBuffer::get(const size_t _at) const
		{
			se_assert(_at < size());
			InstanceData result;
			glm::vec3 temp;
			decomposeTransformationMatrix(buffer[_at], result.position, result.rotation, temp);
			se_assert((fabsf(temp.x - temp.y) < 0.00001f) && (fabsf(temp.z - temp.y) < 0.00001f));
			result.scale = temp.x;
			return result;
		}

		void InstanceBuffer::setChangedAll()
		{
			changedBegin = 0;
			changedEnd = SIZE_MAX;
		}
		void InstanceBuffer::clearChanged()
		{
			changedBegin = SIZE_MAX;
			changedEnd = 0;
		}

		void InstanceBuffer::updateBuffer()
		{
			if (renderers.size() > 0 && size() > 0)
			{
				if (bufferObject == INVALID_RESOURCE_HANDLE || changedBegin >= changedEnd || changedEnd > size())
				{
					safeDestroy<bgfx::DynamicVertexBufferHandle>(bufferObject);
					const bgfx::Memory* bufferMemory = bgfx::copy(data(), static_cast<uint32_t>(bytes()));
					bufferObject = bgfx::createDynamicVertexBuffer(bufferMemory, findVertexLayout(instanceDataAttributes)).idx;
				}
				else
				{
					const uint32_t changedSizeBytes = static_cast<uint32_t>((changedEnd - changedBegin) * sizeof(glm::mat4));
					const bgfx::Memory* bufferMemory = bgfx::copy(data(changedBegin), changedSizeBytes);
					bgfx::DynamicVertexBufferHandle dvbh = { bufferObject };
					bgfx::update(dvbh, static_cast<uint32_t>(changedBegin), bufferMemory);
				}
			}
			else
			{
				safeDestroy<bgfx::DynamicVertexBufferHandle>(bufferObject);
			}
			clearChanged();
			bufferChanged = false;
		}
	}
}
