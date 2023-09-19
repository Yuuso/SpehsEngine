#pragma once

#include "SpehsEngine/Core/Serial/Serial.h"
#include <map>


namespace se
{
	struct SerialTagMap {};
	template<typename K, typename V> struct SerialTag<std::map<K, V>> { typedef SerialTagMap type; };
	template<typename K, typename V> struct SerialTag<std::multimap<K, V>> { typedef SerialTagMap type; };
	template<> template<typename S, typename T>
	static bool se::Serial<SerialTagMap>::serial(S& _serial, T _map)
	{
		typedef typename se::remove_cvref<T>::type MapType;
		typedef typename MapType::key_type KeyType;
		typedef typename MapType::mapped_type ValueType;
		if constexpr (S::getWritingEnabled())
		{
			const uint32_t size = uint32_t(_map.size());
			se_serial(_serial, size, "s");
			size_t counter = 0;
			(void)counter;
			for (const std::pair<const KeyType, ValueType>& pair : _map)
			{
				se_serial(_serial, pair.first, ("k" + std::to_string(counter)).c_str());
				se_serial(_serial, pair.second, ("v" + std::to_string(counter++)).c_str());
			}
			return true;
		}
		else
		{
			MapType temp;
			uint32_t size = 0;
			se_serial(_serial, size, "s");
			for (uint32_t i = 0; i < size; i++)
			{
				KeyType k;
				ValueType v;
				se_serial(_serial, k, ("k" + std::to_string(i)).c_str());
				se_serial(_serial, v, ("v" + std::to_string(i)).c_str());
				temp.emplace(std::make_pair(std::move(k), std::move(v)));
			}
			std::swap(temp, _map);
			return true;
		}
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
