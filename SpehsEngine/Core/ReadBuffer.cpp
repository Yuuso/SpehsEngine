#include "stdafx.h"
#include "ReadBuffer.h"
#include "Log.h"

namespace se
{
	ReadBuffer::ReadBuffer(const void* pointedMemory, const size_t length)
		: data((const unsigned char*)pointedMemory)
		, size(length)
	{

	}

	ReadBuffer::~ReadBuffer()
	{
		//NOTE: do not deallocate data! data is owned by an external source!
	}
		
	void ReadBuffer::translate(const int bytes)
	{
		offset += bytes;
	}

	size_t ReadBuffer::getBytesRemaining() const
	{
		return size - offset;
	}

	void readFromBuffer(ReadBuffer& buffer, std::string& string)
	{
		size_t size;
		buffer.read(size);
		string.resize(size);
		for (size_t i = 0; i < size; i++)
			buffer.read(string[i]);
	}
		
	void readFromBuffer(ReadBuffer& buffer, se::time::Time& time)
	{
		buffer.read(time.value);
	}
}