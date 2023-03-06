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

	template<typename Writer, typename T>
	void writer(Writer& _writer, const std::optional<T>& _optional)
	{
		const bool exists = _optional.has_value();
		se_writer(_writer, exists, "e");
		if (exists)
		{
			se_writer(_writer, _optional.value(), "v");
		}
	}

	template<typename Reader, typename T>
	bool reader(Reader& _reader, std::optional<T>& _optional)
	{
		bool exists = false;
		se_reader(_reader, exists, "e");
		if (exists)
		{
			_optional.emplace();
			se_reader(_reader, _optional.value(), "v");
		}
		else
		{
			_optional.reset();
		}
		return true;
	}
}
