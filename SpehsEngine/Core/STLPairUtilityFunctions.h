#pragma once

#include <stdint.h>
#include <utility>
#include "SpehsEngine/Core/Archive.h"
#include "SpehsEngine/Core/ReadBuffer.h"
#include "SpehsEngine/Core/WriteBuffer.h"


namespace se
{
	template<typename T1, typename T2>
	void writeToBuffer(WriteBuffer& writeBuffer, const std::pair<T1, T2>& pair)
	{
		se_write(writeBuffer, pair.first);
		se_write(writeBuffer, pair.second);
	}

	template<typename T1, typename T2>
	bool readFromBuffer(ReadBuffer& readBuffer, std::pair<T1, T2>& pair)
	{
		se_read(readBuffer, pair.first);
		se_read(readBuffer, pair.second);
		return true;
	}

	template<typename T1, typename T2>
	Archive writeToArchive(const std::pair<T1, T2>& pair)
	{
		Archive archive;
		se_write_to_archive(archive, pair.first);
		se_write_to_archive(archive, pair.second);
		return archive;
	}

	template<typename T1, typename T2>
	bool readFromArchive(const Archive& archive, std::pair<T1, T2>& pair)
	{
		se_read_from_archive(archive, pair.first);
		se_read_from_archive(archive, pair.second);
		return true;
	}
}
