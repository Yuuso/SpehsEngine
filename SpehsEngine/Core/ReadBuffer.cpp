#include "stdafx.h"
#include "SpehsEngine/Core/ReadBuffer.h"


namespace se
{
	LegacyReadBuffer::LegacyReadBuffer(const void* pointedMemory, const size_t length)
		: data((const unsigned char*)pointedMemory)
		, size(length)
	{
	}

	LegacyReadBuffer::~LegacyReadBuffer()
	{
		//NOTE: do not deallocate data! data is owned by an external source!
	}
		
	void LegacyReadBuffer::translate(const int bytes)
	{
		offset += bytes;
	}

	size_t LegacyReadBuffer::getBytesRemaining() const
	{
		return size - offset;
	}
}
