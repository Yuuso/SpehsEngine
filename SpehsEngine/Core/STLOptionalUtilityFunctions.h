#pragma once

#include <stdint.h>
#include <optional>
#include "SpehsEngine/Core/Serial/Serial.h"
#include "SpehsEngine/Core/Archive.h"
#include "SpehsEngine/Core/ReadBuffer.h"
#include "SpehsEngine/Core/WriteBuffer.h"


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

	template<typename T>
	Archive writeToArchive(const std::optional<T>& optional)
	{
		Archive archive;
		const uint8_t hasValue = optional ? 1 : 0;
		se_write_to_archive(archive, hasValue);
		if (hasValue != 0)
		{
			se_write_to_archive(archive, *optional);
		}
		return archive;
	}

	template<typename T>
	bool readFromArchive(const Archive& archive, std::optional<T>& optional)
	{
		uint8_t hasValue = 0;
		se_read_from_archive(archive, hasValue);
		if (hasValue != 0)
		{
			optional.emplace();
			se_read_from_archive(archive, *optional);
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
