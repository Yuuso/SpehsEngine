#pragma once
#include <string>
#include "SpehsEngine/Core/Archive.h"
#include "SpehsEngine/Core/ReadBuffer.h"
#include "SpehsEngine/Core/WriteBuffer.h"

namespace se
{
	template<typename SizeType = size_t>
	void writeToBuffer(WriteBuffer& buffer, const std::string& string)
	{
		static_assert(std::is_integral<SizeType>::value, "SizeType must be integral.");
		se_assert(size_t(std::numeric_limits<SizeType>::max()) >= string.size() && "String size is larger than the maximum of SizeType.");
		const SizeType size = SizeType(string.size());
		buffer.write(size);
		for (SizeType i = 0; i < size; i++)
		{
			buffer.write(string[size_t(i)]);
		}
	}

	template<typename SizeType = size_t>
	bool readFromBuffer(ReadBuffer& buffer, std::string& string)
	{
		static_assert(std::is_integral<SizeType>::value, "SizeType must be integral.");
		SizeType size;
		if (!buffer.read(size))
		{
			return false;
		}
		string.resize(size_t(size));
		for (SizeType i = 0; i < size; i++)
		{
			if (!buffer.read(string[size_t(i)]))
			{
				return false;
			}
		}
		return true;
	}
	
	template<typename SizeType = size_t>
	void writeToArchive(Archive& archive, const std::string& valueName, const std::string& string)
	{
		static_assert(std::is_integral<SizeType>::value, "SizeType must be integral.");
		WriteBuffer writeBuffer;
		writeToBuffer(writeBuffer, string);
		archive.write(valueName, writeBuffer);
	}

	template<typename SizeType = size_t>
	bool readFromArchive(const Archive& archive, const std::string& valueName, std::string& string)
	{
		static_assert(std::is_integral<SizeType>::value, "SizeType must be integral.");
		WriteBuffer writeBuffer;
		if (!archive.read(valueName, writeBuffer))
		{
			return false;
		}
		ReadBuffer readBuffer(writeBuffer[0], writeBuffer.getSize());
		readFromBuffer(readBuffer, string);
		return true;
	}
}
