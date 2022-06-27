#pragma once

#include "SpehsEngine/Core/Archive.h"
#include "SpehsEngine/Core/ReadBuffer.h"
#include "SpehsEngine/Core/TypeTraits.h"
#include "SpehsEngine/Core/WriteBuffer.h"
#include <unordered_set>


namespace se
{
	template<typename ValueType, typename Hasher = std::hash<ValueType>, typename SizeType = uint32_t>
	void writeToBuffer(WriteBuffer& buffer, const std::unordered_set<ValueType, Hasher>& unorderedSet)
	{
		static_assert(std::is_integral<SizeType>::value, "SizeType must be integral.");
		se_assert(size_t(std::numeric_limits<SizeType>::max()) >= unorderedSet.size() && "Unordered set size is larger than the maximum of SizeType.");
		const SizeType size = SizeType(unorderedSet.size());
		buffer.write(size);
		for (const ValueType& value : unorderedSet)
		{
			buffer.write(value);
		}
	}

	template<typename ValueType, typename Hasher = std::hash<ValueType>, typename SizeType = uint32_t>
	bool readFromBuffer(ReadBuffer& buffer, std::unordered_set<ValueType, Hasher>& unorderedSet)
	{
		static_assert(std::is_integral<SizeType>::value, "SizeType must be integral.");
		SizeType size = 0;
		if (!buffer.read(size))
		{
			return false;
		}
		unorderedSet.clear();
		for (SizeType i = 0; i < size; i++)
		{
			ValueType value;
			if (!buffer.read(value))
			{
				return false;
			}
			unorderedSet.insert(value);
		}
		return true;
	}

	template<typename ValueType, typename Hasher = std::hash<ValueType>, typename SizeType = uint32_t>
	Archive writeToArchive(const std::unordered_set<ValueType, Hasher>& unorderedSet)
	{
		static_assert(std::is_integral<SizeType>::value, "SizeType must be integral.");
		Archive archive;
		const SizeType size = SizeType(unorderedSet.size());
		se_write_to_archive(archive, size);
		size_t index = 0;
		for (const ValueType& value : unorderedSet)
		{
			archive.write(std::to_string(index++) + "v", value);
		}
		return archive;
	}

	template<typename ValueType, typename Hasher = std::hash<ValueType>, typename SizeType = uint32_t>
	bool readFromArchive(const Archive& archive, std::unordered_set<ValueType, Hasher>& unorderedSet)
	{
		static_assert(std::is_integral<SizeType>::value, "SizeType must be integral.");
		SizeType size = 0;
		se_read_from_archive(archive, size);
		for (SizeType i = 0; i < size; i++)
		{
			ValueType value;
			if (!archive.read(std::to_string(i) + "v", value))
			{
				return false;
			}
			unorderedSet.insert(value);
		}
		return true;
	}
}

template<typename ValueType, typename Hasher = std::hash<ValueType>, typename SizeType = uint32_t>
inline ValueType* tryFind(std::unordered_set<ValueType, Hasher>& unorderedSet, const ValueType &value)
{
	const std::unordered_set<ValueType, Hasher>::iterator it = unorderedSet.find(value);
	if (it != unorderedSet.end())
	{
		return (ValueType*)&(*it);
	}
	else
	{
		return nullptr;
	}
}

template<typename ValueType, typename Hasher = std::hash<ValueType>, typename SizeType = uint32_t>
inline const ValueType* tryFind(const std::unordered_set<ValueType, Hasher>& unorderedSet, const ValueType &value)
{
	const std::unordered_set<ValueType, Hasher>::const_iterator it = unorderedSet.find(value);
	if (it != unorderedSet.end())
	{
		return (const ValueType*)&(*it);
	}
	else
	{
		return nullptr;
	}
}

template<typename ValueType, typename Hasher = std::hash<ValueType>, typename SizeType = uint32_t>
inline bool findAndErase(std::unordered_set<ValueType, Hasher>& unorderedSet, const ValueType& value)
{
	const std::unordered_set<ValueType, Hasher>::iterator it = unorderedSet.find(value);
	if (it != unorderedSet.end())
	{
		unorderedSet.erase(it);
		return true;
	}
	else
	{
		return false;
	}
}
