#pragma once

#include "SpehsEngine/Core/SE_Assert.h"
#include "SpehsEngine/Core/Murmur3.h"
#include <string>
#include <stack>


namespace se
{
	class ArchiveIdStack
	{
	public:

		static inline constexpr uint32_t getKeyHash(const std::string_view _key)
		{
			return Murmur3::murmurHash3_x86_32(_key.data(), _key.size(), 5);
		}

		template<typename T>
		static constexpr uint64_t getTypeId()
		{
			if constexpr (std::is_floating_point<T>::value)
			{
				return uint64_t(16 + sizeof(T));
			}
			else if constexpr (std::numeric_limits<T>::is_integer && std::is_signed<T>::value)
			{
				return uint64_t(32 + sizeof(T));
			}
			else if constexpr (std::numeric_limits<T>::is_integer && std::is_unsigned<T>::value)
			{
				return uint64_t(48 + sizeof(T));
			}
			else if constexpr (std::is_same<T, ByteView>::value || std::is_same<T, ByteVector>::value)
			{
				return uint64_t(64 + 1);
			}
			else
			{
				return uint64_t(0);
			}
		}

		// Returns a 64-bit id in the current object id stack
		inline uint64_t pushId(const uint64_t _typeId, const std::string_view _key)
		{
			const uint64_t hash = getHash(_typeId, _key);
			idStack.push(hash);
			return hash;
		}

		inline void popId()
		{
			se_assert(!idStack.empty());
			idStack.pop();
		}

		inline bool empty() const
		{
			return idStack.empty();
		}

	private:

		inline uint64_t getHash(const uint64_t _typeId, const std::string_view _key) const
		{
			const uint32_t keyHash32 = getKeyHash(_key);
			return getHash(_typeId, keyHash32);
		}

		inline uint64_t getHash(const uint64_t _typeId, const uint32_t _keyHash32) const
		{
			const uint64_t stackTop64 = idStack.empty() ? 0 : idStack.top();
			constexpr size_t dataSize = sizeof(stackTop64) + sizeof(_keyHash32) + sizeof(_typeId);
			constexpr size_t stackTopOffset = 0;
			constexpr size_t typeIdOffset = stackTopOffset + sizeof(stackTop64);
			constexpr size_t keyHashOffset = typeIdOffset + sizeof(_typeId);
			uint8_t data[dataSize];
			memcpy(data + stackTopOffset, &stackTop64, sizeof(stackTop64));
			memcpy(data + typeIdOffset, &_typeId, sizeof(_typeId));
			memcpy(data + keyHashOffset, &_keyHash32, sizeof(_keyHash32));
			return murmurHash3_x86_32(&data, dataSize, 1337);
		}

		std::stack<uint64_t> idStack;
	};
}
