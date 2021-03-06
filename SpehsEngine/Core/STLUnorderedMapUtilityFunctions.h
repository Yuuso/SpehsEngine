#pragma once

#include "SpehsEngine/Core/Archive.h"
#include "SpehsEngine/Core/ReadBuffer.h"
#include "SpehsEngine/Core/TypeTraits.h"
#include "SpehsEngine/Core/WriteBuffer.h"
#include <unordered_map>


namespace se
{
	template<typename KeyType, typename T, typename Hasher = std::hash<KeyType>, typename SizeType = uint32_t>
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

	template<typename KeyType, typename T, typename Hasher = std::hash<KeyType>, typename SizeType = uint32_t>
	bool readFromBuffer(ReadBuffer& buffer, std::unordered_map<KeyType, T, Hasher>& unorderedMap)
	{
		static_assert(std::is_integral<SizeType>::value, "SizeType must be integral.");
		SizeType size = 0;
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

	template<typename KeyType, typename T, typename Hasher = std::hash<KeyType>, typename SizeType = uint32_t>
	typename std::enable_if<IsUniquePtr<T>::value, Archive>::type writeToArchive(const std::unordered_map<KeyType, T, Hasher>& unorderedMap)
	{
		static_assert(std::is_integral<SizeType>::value, "SizeType must be integral.");
		Archive archive;
		const SizeType size = SizeType(unorderedMap.size());
		se_write_to_archive(archive, size);
		size_t index = 0;
		for (std::unordered_map<KeyType, T, Hasher>::const_iterator it = unorderedMap.begin(); it != unorderedMap.end(); it++)
		{
			archive.write(std::to_string(index) + "k", it->first);
			const bool allocated = it->second.get() != nullptr;
			archive.write(std::to_string(index) + "va", allocated);
			if (allocated)
			{
				archive.write(std::to_string(index) + "vv", *it->second.get());
			}
			index++;
		}
		return archive;
	}

	template<typename KeyType, typename T, typename Hasher = std::hash<KeyType>, typename SizeType = uint32_t>
	typename std::enable_if<IsUniquePtr<T>::value, bool>::type readFromArchive(const Archive& archive, std::unordered_map<KeyType, T, Hasher>& unorderedMap)
	{
		static_assert(std::is_integral<SizeType>::value, "SizeType must be integral.");
		SizeType size = 0;
		se_read_from_archive(archive, size);
		for (SizeType i = 0; i < size; i++)
		{
			KeyType key;
			if (!archive.read(std::to_string(i) + "k", key))
			{
				return false;
			}
			bool allocated = false;
			if (!archive.read(std::to_string(i) + "va", allocated))
			{
				return false;
			}
			if (allocated)
			{
				unorderedMap[key].reset(new T::element_type());
				if (!archive.read(std::to_string(i) + "vv", *unorderedMap[key].get()))
				{
					return false;
				}
			}
		}
		return true;
	}

	template<typename KeyType, typename T, typename Hasher = std::hash<KeyType>, typename SizeType = uint32_t>
	typename std::enable_if<!IsUniquePtr<T>::value, Archive>::type writeToArchive(const std::unordered_map<KeyType, T, Hasher>& unorderedMap)
	{
		static_assert(std::is_integral<SizeType>::value, "SizeType must be integral.");
		Archive archive;
		const SizeType size = SizeType(unorderedMap.size());
		se_write_to_archive(archive, size);
		size_t index = 0;
		for (std::unordered_map<KeyType, T, Hasher>::const_iterator it = unorderedMap.begin(); it != unorderedMap.end(); it++)
		{
			archive.write(std::to_string(index) + "k", it->first);
			archive.write(std::to_string(index) + "v", it->second);
			index++;
		}
		return archive;
	}

	template<typename KeyType, typename T, typename Hasher = std::hash<KeyType>, typename SizeType = uint32_t>
	typename std::enable_if<!IsUniquePtr<T>::value, bool>::type readFromArchive(const Archive& archive, std::unordered_map<KeyType, T, Hasher>& unorderedMap)
	{
		static_assert(std::is_integral<SizeType>::value, "SizeType must be integral.");
		SizeType size = 0;
		se_read_from_archive(archive, size);
		for (SizeType i = 0; i < size; i++)
		{
			KeyType key;
			if (!archive.read(std::to_string(i) + "k", key))
			{
				return false;
			}
			if (!archive.read(std::to_string(i) + "v", unorderedMap[key]))
			{
				return false;
			}
		}
		return true;
	}
}

/*
	Unordered map equal comparison operator.
	Note that this operator only compares the elements of each container, not how they are stored in memory.
	This could result into some issues, for example "identical" containers may result in non-identical stream output.
	If this becomes a concern then you are free to rename this function into something else.
*/
template<typename KeyType, typename ValueType, typename Hasher = std::hash<KeyType>>
bool operator==(const std::unordered_map<KeyType, ValueType, Hasher>& a, const std::unordered_map<KeyType, ValueType, Hasher>& b)
{
	if (a.size() != b.size())
	{
		return false;
	}
	for (std::unordered_map<KeyType, ValueType, Hasher>::const_iterator aIt = a.begin(); aIt != a.end(); aIt++)
	{
		const std::unordered_map<KeyType, ValueType, Hasher>::const_iterator bIt = b.find(aIt->first);
		if (bIt != b.end())
		{
			if (!(aIt->second == bIt->second))
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	return true;
}

template<typename KeyType, typename ValueType, typename Hasher = std::hash<KeyType>>
inline bool findAndErase(std::unordered_map<KeyType, ValueType, Hasher>& unorderedMap, const KeyType& key)
{
	std::unordered_map<KeyType, ValueType, Hasher>::iterator it = unorderedMap.find(key);
	if (it != unorderedMap.end())
	{
		unorderedMap.erase(it);
		return true;
	}
	else
	{
		return false;
	}
}
