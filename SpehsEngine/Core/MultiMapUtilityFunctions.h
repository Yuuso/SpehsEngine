#pragma once

#include "SpehsEngine/Core/Serial/Serial.h"
#include "SpehsEngine/Core/Archive.h"
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

	template<typename KeyType, typename T, typename Compare = std::less<KeyType>, typename SizeType = uint32_t>
	Archive writeToArchive(const std::multimap<KeyType, T, Compare>& multimap)
	{
		static_assert(std::is_integral<SizeType>::value, "SizeType must be integral.");
		Archive archive;
		const SizeType size = SizeType(multimap.size());
		se_write_to_archive(archive, size);
		size_t index = 0;
		for (auto it = multimap.begin(); it != multimap.end(); it++)
		{
			archive.write(std::to_string(index) + "k", it->first);
			archive.write(std::to_string(index) + "v", it->second);
			index++;
		}
		return archive;
	}

	template<typename KeyType, typename T, typename Compare = std::less<KeyType>, typename SizeType = uint32_t>
	bool readFromArchive(const Archive& archive, std::multimap<KeyType, T, Compare>& multimap)
	{
		static_assert(std::is_integral<SizeType>::value, "SizeType must be integral.");
		SizeType size = 0;
		se_read_from_archive(archive, size);
		std::multimap<KeyType, T, Compare> localMultimap;
		for (SizeType i = 0; i < size; i++)
		{
			KeyType key;
			if (!archive.read(std::to_string(i) + "k", key))
			{
				return false;
			}
			const std::multimap<KeyType, T, Compare>::iterator emplaceIt = localMultimap.emplace(key, T());
			if (emplaceIt == localMultimap.end())
			{
				return false;
			}
			if (!archive.read(std::to_string(i) + "v", emplaceIt->second))
			{
				return false;
			}
		}
		std::swap(localMultimap, multimap);
		return true;
	}

	template<typename Writer, typename Key, typename Value>
	void writer(Writer& _writer, const std::multimap<Key, Value>& _map)
	{
		const uint32_t size = uint32_t(_map.size());
		se_writer(_writer, size, "s");
		size_t counter = 0;
		(void)counter;
		for (const std::pair<const Key, Value>& pair : _map)
		{
			se_writer(_writer, pair.first, ("k" + std::to_string(counter)).c_str());
			se_writer(_writer, pair.second, ("v" + std::to_string(counter++)).c_str());
		}
	}
	template<typename Reader, typename Key, typename Value>
	bool reader(Reader& _reader, std::multimap<Key, Value>& _map)
	{
		std::multimap<Key, Value> temp;
		uint32_t size = 0;
		se_reader(_reader, size, "s");
		for (uint32_t i = 0; i < size; i++)
		{
			std::pair<Key, Value> pair;
			se_reader(_reader, pair.first, ("k" + std::to_string(i)).c_str());
			se_reader(_reader, pair.second, ("v" + std::to_string(i)).c_str());
			temp.insert(std::move(pair));
		}
		std::swap(temp, _map);
		return true;
	}
}
