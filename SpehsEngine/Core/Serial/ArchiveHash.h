#pragma once

#include "SpehsEngine/Core/SE_Assert.h"
#include "SpehsEngine/Core/Murmur3.h"
#include "SpehsEngine/Core/ByteView.h"


namespace se
{
	template<typename T>
	static constexpr uint32_t getArchiveTypeId()
	{
		if constexpr (std::is_floating_point<T>::value)
		{
			return uint32_t(16 + sizeof(T));
		}
		else if constexpr (std::numeric_limits<T>::is_integer && std::is_signed<T>::value)
		{
			return uint32_t(32 + sizeof(T));
		}
		else if constexpr (std::numeric_limits<T>::is_integer && std::is_unsigned<T>::value)
		{
			return uint32_t(48 + sizeof(T));
		}
		else if constexpr (std::is_same<T, ByteVector>::value || std::is_same<T, ByteView>::value)
		{
			return uint32_t(64 + 1);
		}
		else if constexpr (IsStaticByteView<T>::value || IsConstStaticByteView<T>::value)
		{
			return uint32_t(80 + T::getSize());
		}
		else
		{
			return uint32_t(0);
		}
	}

	template<typename T>
	constexpr uint32_t getArchiveHash(const std::string_view _key)
	{
		const uint32_t typeId = getArchiveTypeId<T>();
		return Murmur3::murmurHash3_x86_32(_key.data(), _key.size(), typeId);
	}
}
