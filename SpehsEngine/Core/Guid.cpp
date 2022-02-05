#include "stdafx.h"
#include "SpehsEngine/Core/Guid.h"

#include <functional>
#include "SpehsEngine/Core/WriteBuffer.h"
#include "SpehsEngine/Core/ReadBuffer.h"
#include "SpehsEngine/Core/StringUtilityFunctions.h"


namespace se
{
	size_t Guid::HashFunctor::operator()(const Guid& guid) const
	{
		return std::hash<uint64_t>()(guid.toU64());
	}
	
	void Guid::write(se::WriteBuffer& writeBuffer) const
	{
		const uint64_t u64 = toU64();
		se_write(writeBuffer, u64);
	}

	bool Guid::read(se::ReadBuffer& readBuffer)
	{
		uint64_t u64 = 0;
		se_read(readBuffer, u64);
		(uint64_t&)(*this) = u64;
		return true;
	}

	std::string Guid::toString() const
	{
		return "[f:" + toHexString(flags) + ", m:" + toHexString(metadata) + ", i:" + std::to_string(index) + "]";
	}
}
