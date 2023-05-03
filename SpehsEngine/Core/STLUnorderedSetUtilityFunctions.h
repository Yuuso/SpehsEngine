#pragma once

#include "SpehsEngine/Core/ReadBuffer.h"
#include "SpehsEngine/Core/TypeTraits.h"
#include "SpehsEngine/Core/WriteBuffer.h"
#include <unordered_set>
#include <set>


namespace se
{
	struct SerialTagSet {};
	template<typename T> struct SerialTag<std::unordered_set<T>> { typedef SerialTagSet type; };
	template<typename T> struct SerialTag<std::set<T>> { typedef SerialTagSet type; };
	template<> template<typename S, typename T>
	static bool se::Serial<SerialTagSet>::serial(S& _serial, T _set)
	{
		typedef typename se::remove_cvref<T>::type SetType;
		typedef typename SetType::value_type ValueType;
		if constexpr (S::getWritingEnabled())
		{
			const uint32_t size = uint32_t(_set.size());
			se_serial(_serial, size, "s");
			size_t counter = 0;
			(void)counter;
			for (const ValueType& value : _set)
			{
				se_serial(_serial, value, std::to_string(counter++));
			}
			return true;
		}
		else
		{
			SetType temp;
			uint32_t size = 0;
			se_serial(_serial, size, "s");
			for (uint32_t i = 0; i < size; i++)
			{
				ValueType value;
				se_serial(_serial, value, std::to_string(i));
				temp.emplace(std::move(value));
			}
			std::swap(temp, _set);
			return true;
		}
	}

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
