#pragma once
#include <unordered_map>
#include "SpehsEngine/Core/Archive.h"
#include "SpehsEngine/Core/ReadBuffer.h"
#include "SpehsEngine/Core/WriteBuffer.h"

namespace se
{
	template<typename KeyType, typename T, typename Hasher = std::hash<KeyType>, typename SizeType = size_t>
	void writeToBuffer(WriteBuffer& buffer, const std::unordered_map<KeyType, T, Hasher>& unorderedMap)
	{
		static_assert(std::is_integral<SizeType>::value, "SizeType must be integral.");
		se_assert(size_t(std::numeric_limits<SizeType>::max()) >= unorderedMap.size() && "Unordered map size is larger than the maximum of SizeType.");
		const SizeType size = SizeType(unorderedMap.size());
		buffer.write(size);
		for (std::unordered_map<KeyType, T, Hasher>::const_iterator it = unorderedMap.begin(); it != unorderedMap.end(); it++)
		{
			buffer.write(it->first);
			buffer.write(it->second);
		}
	}

	template<typename KeyType, typename T, typename Hasher = std::hash<KeyType>, typename SizeType = size_t>
	bool readFromBuffer(ReadBuffer& buffer, std::unordered_map<KeyType, T, Hasher>& unorderedMap)
	{
		static_assert(std::is_integral<SizeType>::value, "SizeType must be integral.");
		SizeType size;
		if (!buffer.read(size))
		{
			return false;
		}
		unorderedMap.clear();
		for (SizeType i = 0; i < size; i++)
		{
			KeyType key;
			if (!buffer.read(key))
			{
				return false;
			}
			T& value = unorderedMap[key];
			if (!buffer.read(value))
			{
				return false;
			}
		}
		return true;
	}

	template<typename KeyType, typename T, typename Hasher = std::hash<KeyType>, typename SizeType = size_t>
	void writeToArchive(Archive& archive, const std::string& valueName, const std::unordered_map<KeyType, T, Hasher>& unorderedMap)
	{
		static_assert(std::is_integral<SizeType>::value, "SizeType must be integral.");
		WriteBuffer writeBuffer;
		writeToBuffer(writeBuffer, unorderedMap);
		archive.write(valueName, writeBuffer);
	}

	template<typename KeyType, typename T, typename Hasher = std::hash<KeyType>, typename SizeType = size_t>
	bool readFromArchive(const Archive& archive, const std::string& valueName, std::unordered_map<KeyType, T, Hasher>& unorderedMap)
	{
		static_assert(std::is_integral<SizeType>::value, "SizeType must be integral.");
		WriteBuffer writeBuffer;
		if (!archive.read(valueName, writeBuffer))
		{
			return false;
		}
		ReadBuffer readBuffer(writeBuffer[0], writeBuffer.getSize());
		readFromBuffer(readBuffer, unorderedMap);
		return true;
		return true;
	}
}
