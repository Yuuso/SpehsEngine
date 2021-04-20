#include "stdafx.h"
#include "SpehsEngine/Graphics/InstanceBuffer.h"

#include "bgfx/bgfx.h"
#include "SpehsEngine/Graphics/Internal/InternalUtilities.h"
#include "SpehsEngine/Math/GLMMatrixUtilityFunctions.h"


namespace se
{
	namespace graphics
	{
		static constexpr VertexAttributeFlagsType transformDataAttributes =
			  VertexAttribute::Data0
			| VertexAttribute::Data1
			| VertexAttribute::Data2
			| VertexAttribute::Data3;
		static constexpr VertexAttributeFlagsType billboardDataAttributes =
			  VertexAttribute::Data0
			| VertexAttribute::Data1;

		static_assert(
			  sizeof(VertexAttribute::Data0Type)
			+ sizeof(VertexAttribute::Data1Type)
			+ sizeof(VertexAttribute::Data2Type)
			+ sizeof(VertexAttribute::Data3Type)
			== sizeof(glm::mat4), "Invalid vertex attribute data types for instancing!");
		static_assert(
			  sizeof(VertexAttribute::Data0Type)
			+ sizeof(VertexAttribute::Data1Type)
			== sizeof(glm::vec4) * 2, "Invalid vertex attribute data types for instancing!");

		InstanceBuffer::InstanceBuffer(const InstanceBufferType _type)
			: type(_type)
		{
		}
		InstanceBuffer::~InstanceBuffer()
		{
			safeDestroy<bgfx::DynamicVertexBufferHandle>(bufferObject);
		}

		InstanceBuffer::InstanceBuffer(const InstanceBuffer& _other)
			: BufferObject(_other)
			, buffer(_other.buffer)
			, type(_other.type)
		{
		}
		InstanceBuffer& InstanceBuffer::operator=(const InstanceBuffer& _other)
		{
			se_assert(type == _other.type);
			BufferObject::operator=(_other);
			buffer = _other.buffer;
			return *this;
		}

		InstanceBuffer::InstanceBuffer(InstanceBuffer&& _other)
			: BufferObject(std::move(_other))
			, buffer(std::move(_other.buffer))
			, type(_other.type)
		{
		}
		InstanceBuffer& InstanceBuffer::operator=(InstanceBuffer&& _other)
		{
			se_assert(type == _other.type);
			BufferObject::operator=(std::move(_other));
			buffer = std::move(_other.buffer);
			return *this;
		}


		const size_t InstanceBuffer::size() const
		{
			return buffer.size() / instanceBytes();
		}
		const size_t InstanceBuffer::bytes() const
		{
			return buffer.size();
		}
		void InstanceBuffer::resize(const size_t _size)
		{
			buffer.resize(_size * instanceBytes());
			setChangedAll();
			bufferChanged = true;
		}
		void InstanceBuffer::grow(const size_t _amount)
		{
			resize(size() + _amount);
		}
		void InstanceBuffer::pushBack(const TransformInstanceData& _data)
		{
			const size_t index = size();
			grow(1);
			set(index, _data);
			setChangedAll();
			bufferChanged = true;
		}
		void InstanceBuffer::pushBack(const BillboardInstanceData& _data)
		{
			const size_t index = size();
			grow(1);
			set(index, _data);
			setChangedAll();
			bufferChanged = true;
		}
		void InstanceBuffer::erase(const size_t _begin, const size_t _end)
		{
			buffer.erase(buffer.begin() + _begin * instanceBytes(), buffer.begin() + _end * instanceBytes());
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

		void InstanceBuffer::set(const size_t _at, const TransformInstanceData& _data)
		{
			se_assert(_at < size());
			const size_t atBytes = _at * instanceBytes();
			const glm::mat4 temp = constructTransformationMatrix(_data.position, _data.rotation, glm::vec3(_data.scale));
			memcpy(&buffer[atBytes], &temp, sizeof(temp));
			changedBytesBegin = std::min(changedBytesBegin, atBytes);
			changedBytesBegin = std::max(changedBytesEnd, atBytes + 1);
			bufferChanged = true;
		}
		void InstanceBuffer::set(const size_t _at, const BillboardInstanceData& _data)
		{
			se_assert(_at < size());
			const size_t atBytes = _at * instanceBytes();
			memcpy(&buffer[atBytes], &_data, sizeof(_data));
			changedBytesBegin = std::min(changedBytesBegin, atBytes);
			changedBytesBegin = std::max(changedBytesEnd, atBytes + 1);
			bufferChanged = true;
		}
		const TransformInstanceData InstanceBuffer::getTransformData(const size_t _at) const
		{
			se_assert(_at < size());
			TransformInstanceData result;
			glm::vec3 temp;
			const glm::mat4* mtx = reinterpret_cast<const glm::mat4*>(&buffer[_at * instanceBytes()]);
			decomposeTransformationMatrix(*mtx, result.position, result.rotation, temp);
			se_assert((fabsf(temp.x - temp.y) < 0.00001f) && (fabsf(temp.z - temp.y) < 0.00001f));
			result.scale = temp.x;
			return result;
		}
		const BillboardInstanceData InstanceBuffer::getBillboardData(const size_t _at) const
		{
			se_assert(_at < size());
			const BillboardInstanceData* result = reinterpret_cast<const BillboardInstanceData*>(&buffer[_at * instanceBytes()]);
			return *result;
		}

		void InstanceBuffer::setChangedAll()
		{
			changedBytesBegin = 0;
			changedBytesEnd = SIZE_MAX;
		}
		void InstanceBuffer::clearChanged()
		{
			changedBytesBegin = SIZE_MAX;
			changedBytesEnd = 0;
		}
		size_t InstanceBuffer::instanceBytes() const
		{
			switch (type)
			{
				case InstanceBufferType::Transform:
					return sizeof(glm::mat4);
				case InstanceBufferType::Billboard:
					return sizeof(glm::vec4) * 2;
			}
			se_assert_m(false, "InstanceBufferType not set!");
			return 1;
		}
		static VertexAttributeFlagsType attributesType(const InstanceBufferType _type)
		{
			switch (_type)
			{
				case InstanceBufferType::Transform:
					return transformDataAttributes;
				case InstanceBufferType::Billboard:
					return billboardDataAttributes;
			}
			se_assert_m(false, "Invalid InstanceBufferType!");
			return 0;
		}

		void InstanceBuffer::updateBuffer()
		{
			if (renderers.size() > 0 && size() > 0)
			{
				if (bufferObject == INVALID_RESOURCE_HANDLE || changedBytesBegin >= changedBytesEnd || changedBytesEnd > size())
				{
					safeDestroy<bgfx::DynamicVertexBufferHandle>(bufferObject);
					const bgfx::Memory* bufferMemory = bgfx::copy(data(), static_cast<uint32_t>(bytes()));
					bufferObject = bgfx::createDynamicVertexBuffer(bufferMemory, findVertexLayout(attributesType(type))).idx;
				}
				else
				{
					const uint32_t changedSizeBytes = static_cast<uint32_t>((changedBytesEnd - changedBytesBegin) * sizeof(glm::mat4));
					const bgfx::Memory* bufferMemory = bgfx::copy(data(changedBytesBegin), changedSizeBytes);
					bgfx::DynamicVertexBufferHandle dvbh = { bufferObject };
					bgfx::update(dvbh, static_cast<uint32_t>(changedBytesBegin), bufferMemory);
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
