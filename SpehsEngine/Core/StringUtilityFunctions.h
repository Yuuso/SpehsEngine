#pragma once
#include <string>
#include "SpehsEngine/Core/Archive.h"
#include "SpehsEngine/Core/SE_Assert.h"
#include "SpehsEngine/Core/ReadBuffer.h"
#include "SpehsEngine/Core/WriteBuffer.h"

namespace se
{
	template<typename SizeType = uint32_t>
	void writeToBuffer(WriteBuffer& writeBuffer, const std::string& string)
	{
		static_assert(std::is_integral<SizeType>::value, "SizeType must be integral.");
		se_assert(size_t(std::numeric_limits<SizeType>::max()) >= string.size() && "String size is larger than the maximum of SizeType.");
		const SizeType size = SizeType(string.size());
		writeBuffer.write(size);
		for (SizeType i = 0; i < size; i++)
		{
			writeBuffer.write(string[size_t(i)]);
		}
	}

	template<typename SizeType = uint32_t>
	bool readFromBuffer(ReadBuffer& readBuffer, std::string& string)
	{
		static_assert(std::is_integral<SizeType>::value, "SizeType must be integral.");
		SizeType size = 0;
		if (!readBuffer.read(size))
		{
			return false;
		}
		if (readBuffer.getBytesRemaining() < size)
		{
			return false;
		}
		string.resize(size_t(size));
		for (SizeType i = 0; i < size; i++)
		{
			if (!readBuffer.read(string[size_t(i)]))
			{
				return false;
			}
		}
		return true;
	}
	
	template<typename SizeType = uint32_t>
	Archive writeToArchive(const std::string& string)
	{
		static_assert(std::is_integral<SizeType>::value, "SizeType must be integral.");
		WriteBuffer writeBuffer;
		writeToBuffer<SizeType>(writeBuffer, string);
		Archive archive;
		se_write_to_archive(archive, writeBuffer);
		return archive;
	}

	template<typename SizeType = uint32_t>
	bool readFromArchive(const Archive& archive, std::string& string)
	{
		static_assert(std::is_integral<SizeType>::value, "SizeType must be integral.");
		WriteBuffer writeBuffer;
		se_read_from_archive(archive, writeBuffer);
		if (writeBuffer.getSize() > 0)
		{
			ReadBuffer readBuffer(writeBuffer[0], writeBuffer.getSize());
			return readFromBuffer<SizeType>(readBuffer, string);
		}
		else
		{
			return true;
		}
	}

	bool doesStartWith(const std::string& string, const std::string& searchParameter);
	bool doesEndWith(const std::string& string, const std::string& searchParameter);

	/*
		Returns a nicely formated time length string.
		Presicion: how many digits to display.
	*/
	std::string toTimeLengthString(const time::Time& time, const size_t precision);
}
