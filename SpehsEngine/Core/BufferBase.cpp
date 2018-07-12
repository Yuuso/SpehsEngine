#include "stdafx.h"
#include "BufferBase.h"

namespace se
{
	BufferBase::BufferBase()
	{

	}

	BufferBase::~BufferBase()
	{

	}

	size_t BufferBase::getOffset() const
	{
		return offset;
	}
}