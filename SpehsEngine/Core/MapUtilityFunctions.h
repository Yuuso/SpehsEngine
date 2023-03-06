#pragma once

#include "SpehsEngine/Core/Serial/Serial.h"
#include <map>


namespace se
{
	template<typename Writer, typename Key, typename Value>
	void writer(Writer& _writer, const std::map<Key, Value>& _map)
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
	bool reader(Reader& _reader, std::map<Key, Value>& _map)
	{
		std::map<Key, Value> temp;
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

template<typename KeyType, typename ValueType, typename Hasher = std::hash<KeyType>>
inline bool findAndErase(std::map<KeyType, ValueType, Hasher>& map, const KeyType& key)
{
	typename std::map<KeyType, ValueType, Hasher>::iterator it = map.find(key);
	if (it != map.end())
	{
		map.erase(it);
		return true;
	}
	else
	{
		return false;
	}
}

template<typename KeyType, typename ValueType, typename Hasher = std::hash<KeyType>>
inline ValueType* tryFind(std::map<KeyType, ValueType, Hasher>& map, const KeyType& key)
{
	typename std::map<KeyType, ValueType, Hasher>::iterator it = map.find(key);
	if (it != map.end())
	{
		return &it->second;
	}
	else
	{
		return nullptr;
	}
}

template<typename KeyType, typename ValueType, typename Hasher = std::hash<KeyType>>
inline const ValueType* tryFind(const std::map<KeyType, ValueType, Hasher>& map, const KeyType& key)
{
	typename std::map<KeyType, ValueType, Hasher>::const_iterator it = map.find(key);
	if (it != map.end())
	{
		return &it->second;
	}
	else
	{
		return nullptr;
	}
}
