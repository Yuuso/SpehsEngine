#pragma once

#include "SpehsEngine/Core/TypeTraits.h"
#include <unordered_map>


namespace se
{
	struct SerialTagUnorderedMap {};
	template<typename K, typename V> struct SerialTag<std::unordered_map<K, V>> { typedef SerialTagUnorderedMap type; };
	template<> template<typename S, typename T>
	static bool Serial<SerialTagUnorderedMap>::serial(S& _serial, T _map)
	{
		typedef typename remove_cvref<T>::type MapType;
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
	for (typename std::unordered_map<KeyType, ValueType, Hasher>::const_iterator aIt = a.begin(); aIt != a.end(); aIt++)
	{
		const typename std::unordered_map<KeyType, ValueType, Hasher>::const_iterator bIt = b.find(aIt->first);
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
	typename std::unordered_map<KeyType, ValueType, Hasher>::iterator it = unorderedMap.find(key);
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

template<typename KeyType, typename ValueType, typename Hasher = std::hash<KeyType>>
inline ValueType* tryFind(std::unordered_map<KeyType, ValueType, Hasher>& unorderedMap, const KeyType& key)
{
	typename std::unordered_map<KeyType, ValueType, Hasher>::iterator it = unorderedMap.find(key);
	if (it != unorderedMap.end())
	{
		return &it->second;
	}
	else
	{
		return nullptr;
	}
}

template<typename KeyType, typename ValueType, typename Hasher = std::hash<KeyType>>
inline const ValueType* tryFind(const std::unordered_map<KeyType, ValueType, Hasher>& unorderedMap, const KeyType& key)
{
	typename std::unordered_map<KeyType, ValueType, Hasher>::const_iterator it = unorderedMap.find(key);
	if (it != unorderedMap.end())
	{
		return &it->second;
	}
	else
	{
		return nullptr;
	}
}
