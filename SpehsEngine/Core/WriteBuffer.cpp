#include "stdafx.h"
#include "SpehsEngine/Core/WriteBuffer.h"


namespace se
{
	void LegacyWriteBuffer::translate(const int bytes)
	{
		se_assert(bytes > 0 || offset >= size_t(std::abs(bytes)));
		if (offset + bytes > data.size())
		{
			data.resize(offset + bytes);
		}
		offset += bytes;
	}

	void LegacyWriteBuffer::setOffset(const size_t _offset)
	{
		translate(int(_offset) - int(getOffset()));
	}
	
	void LegacyWriteBuffer::resize(const size_t size)
	{
		data.resize(size);
		if (offset > size)
			offset = size;
	}

	void LegacyWriteBuffer::reserve(const size_t capacity)
	{
		se_assert(capacity >= data.capacity());
		data.reserve(capacity);
	}

	void LegacyWriteBuffer::clear()
	{
		data.clear();
		offset = 0;
	}

	bool LegacyWriteBuffer::extend(const size_t addedBytes)
	{
		if (addedBytes == 0)
			return true;

		data.reserve(getCapacity() + addedBytes);

		return true;
	}
}
