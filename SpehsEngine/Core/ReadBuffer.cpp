#include "ReadBuffer.h"
#include "Log.h"

namespace spehs
{
	ReadBuffer::ReadBuffer(const void* pointedMemory, const size_t length)
		: data((const unsigned char*)pointedMemory)
		, capacity(length)
	{
		SPEHS_ASSERT(pointedMemory);
		SPEHS_ASSERT(length > 0);
	}

	ReadBuffer::~ReadBuffer()
	{
		//NOTE: do not deallocate data! data is owned by an external source!
	}
		
	void ReadBuffer::translate(const int translationOffset)
	{
		offset += translationOffset;
	}

	size_t ReadBuffer::getBytesRemaining() const
	{
		return capacity - offset;
	}

	void readFromBuffer(ReadBuffer& buffer, std::string& string)
	{
		size_t size;
		buffer.read(size);
		string.resize(size);
		for (size_t i = 0; i < size; i++)
			buffer.read(string[i]);
	}
		
	void readFromBuffer(ReadBuffer& buffer, spehs::time::Time& time)
	{
		buffer.read(time.value);
	}
}