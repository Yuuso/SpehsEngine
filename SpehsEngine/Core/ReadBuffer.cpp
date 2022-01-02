#include "stdafx.h"
#include "SpehsEngine/Core/ReadBuffer.h"


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
}
