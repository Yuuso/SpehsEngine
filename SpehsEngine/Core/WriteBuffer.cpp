#include "stdafx.h"
#include "WriteBuffer.h"
#include "Log.h"

namespace se
{
	WriteBuffer::WriteBuffer()
	{

	}

	WriteBuffer::~WriteBuffer()
	{

	}
		
	void WriteBuffer::reserve(const size_t capacity)
	{
		SPEHS_ASSERT(capacity >= data.capacity());
		data.reserve(capacity);
	}

	bool WriteBuffer::extend(const size_t addedBytes)
	{
		if (addedBytes == 0)
			return true;

		data.reserve(getCapacity() + addedBytes);

		return true;
	}

	void writeToBuffer(WriteBuffer& buffer, const std::string& string)
	{
		const size_t size = string.size();
		buffer.write(size);
		for (size_t i = 0; i < size; i++)
			buffer.write(string[i]);
	}

	void writeToBuffer(WriteBuffer& buffer, const se::time::Time& time)
	{
		buffer.write(time.value);
	}
}