#pragma once

#include "SpehsEngine/Core/Serial/Serial.h"
#include "SpehsEngine/Core/ReadBuffer.h"
#include "SpehsEngine/Core/TypeTraits.h"
#include "SpehsEngine/Core/WriteBuffer.h"
#include <map>


namespace se
{
	template<typename KeyType, typename T, typename Compare = std::less<KeyType>, typename SizeType = uint32_t>
	void writeToBuffer(WriteBuffer& buffer, const std::multimap<KeyType, T, Compare>& multimap)
	{
		static_assert(std::is_integral<SizeType>::value, "SizeType must be integral.");
		se_assert(size_t(std::numeric_limits<SizeType>::max()) >= multimap.size() && "Multimap size is larger than the maximum of SizeType.");
		const SizeType size = SizeType(multimap.size());
		buffer.write(size);
		for (auto it = multimap.begin(); it != multimap.end(); it++)
		{
			buffer.write(it->first);
			buffer.write(it->second);
		}
	}

	template<typename KeyType, typename T, typename Compare = std::less<KeyType>, typename SizeType = uint32_t>
	bool readFromBuffer(ReadBuffer& buffer, std::multimap<KeyType, T, Compare>& multimap)
	{
		static_assert(std::is_integral<SizeType>::value, "SizeType must be integral.");
		SizeType size = 0;
		if (!buffer.read(size))
		{
			return false;
		}
		multimap.clear();
		for (SizeType i = 0; i < size; i++)
		{
			KeyType key;
			if (!buffer.read(key))
			{
				return false;
			}
			T& value = multimap[key];
			if (!buffer.read(value))
			{
				return false;
			}
		}
		return true;
	}
}
