#pragma once

#include "SpehsEngine/Core/Serial/Serial.h"
#include "SpehsEngine/Core/ReadBuffer.h"
#include "SpehsEngine/Core/WriteBuffer.h"
#include <stdint.h>
#include <optional>


namespace se
{
	template<typename T>
	void writeToBuffer(WriteBuffer& writeBuffer, const std::optional<T>& optional)
	{
		const uint8_t hasValue = optional.has_value() ? 1 : 0;
		writeBuffer.write(hasValue);
		if (optional)
		{
			se_write(writeBuffer, *optional);
		}
	}

	template<typename T>
	bool readFromBuffer(ReadBuffer& readBuffer, std::optional<T>& optional)
	{
		uint8_t hasValue = 0u;
		se_read(readBuffer, hasValue);
		if (hasValue != 0)
		{
			optional.emplace();
			se_read(readBuffer, *optional);
		}
		return true;
	}

	struct SerialTagOptional {};
	template<typename T> struct SerialTag<std::optional<T>> { typedef SerialTagOptional type; };
	template<> template<typename S, typename T>
	static bool Serial<SerialTagOptional>::serial(S& _serial, T _optional)
	{
		if constexpr (S::getWritingEnabled())
		{
			const bool exists = _optional.has_value();
			se_serial(_serial, exists, "e");
			if (exists)
			{
				se_serial(_serial, _optional.value(), "v");
			}
			return true;
		}
		else
		{
			bool exists = false;
			se_serial(_serial, exists, "e");
			if (exists)
			{
				_optional.emplace();
				se_serial(_serial, _optional.value(), "v");
			}
			else
			{
				_optional.reset();
			}
			return true;
		}
	}
}
