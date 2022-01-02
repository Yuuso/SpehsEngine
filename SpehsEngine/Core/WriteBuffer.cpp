#include "stdafx.h"
#include "SpehsEngine/Core/WriteBuffer.h"

#include "SpehsEngine/Core/STLVectorUtilityFunctions.h"


namespace se
{
	WriteBuffer::WriteBuffer()
	{

	}

	WriteBuffer::~WriteBuffer()
	{

	}

	void WriteBuffer::write(se::WriteBuffer& writeBuffer) const
	{
		writeBuffer.write(data);
	}

	bool WriteBuffer::read(se::ReadBuffer& readBuffer)
	{
		se_read(readBuffer, data);
		return true;
	}

	void WriteBuffer::translate(const int bytes)
	{
		se_assert(bytes > 0 || offset >= size_t(std::abs(bytes)));
		if (offset + bytes > data.size())
		{
			data.resize(offset + bytes);
		}
		offset += bytes;
	}

	void WriteBuffer::setOffset(const size_t _offset)
	{
		translate(int(_offset) - int(getOffset()));
	}
	
	void WriteBuffer::resize(const size_t size)
	{
		data.resize(size);
		if (offset > size)
			offset = size;
	}

	void WriteBuffer::reserve(const size_t capacity)
	{
		se_assert(capacity >= data.capacity());
		data.reserve(capacity);
	}

	bool WriteBuffer::extend(const size_t addedBytes)
	{
		if (addedBytes == 0)
			return true;

		data.reserve(getCapacity() + addedBytes);

		return true;
	}
}
