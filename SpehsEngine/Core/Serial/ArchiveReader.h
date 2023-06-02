#pragma once

#include "SpehsEngine/Core/Serial/BinaryReader.h"
#include "SpehsEngine/Core/Serial/ArchiveHash.h"
#include "SpehsEngine/Core/UnorderedMapUtilityFunctions.h"
#include <stack>
#include <vector>


namespace se
{
	class ArchiveReader
	{
	public:

		static inline constexpr bool getKeyEnabled() { return true; }
		static inline constexpr bool getWritingEnabled() { return false; }
		static inline constexpr bool getReadingEnabled() { return true; }

		ArchiveReader(const uint8_t* const _data, const size_t _size);

		template<typename T>
		inline bool serial(T& _value, const std::string_view _key);

		inline uint8_t getVersion() { return version; }

	private:

		struct Impl;

		struct Value
		{
			uint32_t index = 0;
			uint32_t size = 0;
		};

		struct Container
		{
			std::unordered_map<uint32_t, Value> values;
			std::unordered_map<uint32_t, Container> containers;
		};

		const Container* findContainer(const uint32_t _hash) const
		{
			if (containerStack.empty())
			{
				return tryFind(rootContainer.containers, _hash);
			}
			else if (containerStack.top())
			{
				return tryFind(containerStack.top()->containers, _hash);
			}
			else
			{
				return nullptr;
			}
		}

		const Value* findValue(const uint32_t _hash) const
		{
			if (containerStack.empty())
			{
				return tryFind(rootContainer.values, _hash);
			}
			else if (containerStack.top())
			{
				return tryFind(containerStack.top()->values, _hash);
			}
			else
			{
				return nullptr;
			}
		}

		Container rootContainer;
		std::stack<const Container*> containerStack;
		std::vector<uint8_t> valueData;
		uint8_t version = 0;
	};

	template<typename T>
	inline bool ArchiveReader::serial(T& _value, const std::string_view _key)
	{
		if constexpr (std::is_enum<T>::value)
		{
			return serial<std::underlying_type<T>::type>((typename std::underlying_type<T>::type&)_value, _key);
		}
		else if constexpr (!std::is_class<T>::value)
		{
			const uint32_t hash = getArchiveHash<T>(_key);
			if (const Value* const value = findValue(hash))
			{
				se_assert(size_t(value->size) == sizeof(T));
				BinaryReader binaryReader(valueData.data() + value->index, size_t(value->size));
				const bool readResult = binaryReader.serial(_value);
				se_assert(readResult);
				(void)readResult;
			}
			return true;
		}
		else if constexpr (std::is_same<T, ByteVector>::value)
		{
			// Read dynamic size data block
			const uint32_t hash = getArchiveHash<T>(_key);
			if (const Value* const value = findValue(hash))
			{
				std::vector<std::byte> bytes;
				_value.swap(bytes);
				bytes.clear();
				bytes.resize(size_t(value->size));
				memcpy(bytes.data(), valueData.data() + value->index, size_t(value->size));
				_value.swap(bytes);
			}
			else
			{
				_value.clear();
			}
			return true;
		}
		else if constexpr (IsStaticByteView<T>::value)
		{
			// Read static size data block
			const uint32_t hash = getArchiveHash<T>(_key);
			if (const Value* const value = findValue(hash))
			{
				if (value->size == T::getSize())
				{
					memcpy(_value.getData(), valueData.data() + value->index, T::getSize());
				}
				else
				{
					memset(_value.getData(), 0, T::getSize());
					return false;
				}
			}
			else
			{
				memset(_value.getData(), 0, T::getSize());
			}
			return true;
		}
		else
		{
			// Free reader
			const uint32_t hash = getArchiveHash<T>(_key);
			containerStack.push(findContainer(hash));
			const bool result = Serial<SerialTag<T>::type>::template serial<ArchiveReader, T&>(*this, _value);
			containerStack.pop();
			return result;
		}
	}
}
