#pragma once

#include "SpehsEngine/Core/SE_Assert.h"
#include "SpehsEngine/Core/Murmur3.h"
#include "SpehsEngine/Core/ByteView.h"


namespace se
{
	struct SerialTypeId
	{
		constexpr SerialTypeId() noexcept = default;
		constexpr SerialTypeId(const uint32_t _value) noexcept : value(_value) {}
		uint32_t value = 0;
	};

	template<typename T>
	struct SerialKey
	{
		SerialKey(const std::string_view _key) noexcept;
		SerialKey(const char *const _key) noexcept;
		constexpr SerialKey(const uint32_t _value) noexcept : value(_value) {}
		uint32_t value = 0;
	};

	template<typename T>
	static constexpr SerialTypeId getSerialTypeId()
	{
		typedef typename se::remove_cvref<T>::type U;
		if constexpr (std::is_enum<U>::value)
		{
			return getSerialTypeId<typename std::underlying_type<U>::type>();
		}
		else if constexpr (std::is_floating_point<U>::value)
		{
			return SerialTypeId(16 + sizeof(U));
		}
		else if constexpr (std::numeric_limits<U>::is_integer && std::is_signed<U>::value)
		{
			return SerialTypeId(32 + sizeof(U));
		}
		else if constexpr (std::numeric_limits<U>::is_integer && std::is_unsigned<U>::value)
		{
			return SerialTypeId(48 + sizeof(U));
		}
		else if constexpr (std::is_same<U, ByteVector>::value || std::is_same<U, ByteView>::value)
		{
			return SerialTypeId(64 + 1);
		}
		else if constexpr (IsStaticByteView<U>::value || IsConstStaticByteView<U>::value)
		{
			return SerialTypeId(80 + uint32_t(U::getSize()));
		}
		else
		{
			return SerialTypeId(0);
		}
	}

	template<typename T>
	constexpr uint32_t calculateSerialKeyValue(const std::string_view _key)
	{
		const SerialTypeId typeId = getSerialTypeId<T>();
		return Murmur3::impl(_key.data(), _key.size(), typeId.value);
	}

	template<typename T>
	SerialKey<T>::SerialKey(const std::string_view _key) noexcept
		: value(calculateSerialKeyValue<T>(_key))
	{
	}

	template<typename T>
	SerialKey<T>::SerialKey(const char* const _key) noexcept
		: value(calculateSerialKeyValue<T>(_key))
	{
	}
}
