#pragma once

#include "boost/format.hpp"
#include "SpehsEngine/Core/Archive.h"
#include "SpehsEngine/Core/SE_Assert.h"
#include "SpehsEngine/Core/ReadBuffer.h"
#include "SpehsEngine/Core/WriteBuffer.h"
#include <string>


namespace se
{
	inline std::string formatStringImpl(boost::format& f)
	{
		return boost::str(f);
	}
	template<typename T, typename... Args>
	inline std::string formatStringImpl(boost::format& f, T&& t, Args&&... args)
	{
		return formatStringImpl(f % std::forward<T>(t), std::forward<Args>(args)...);
	}
	template<typename... Arguments>
	inline std::string formatString(const std::string& format, Arguments&&... args)
	{
		boost::format boostFormat(format.c_str());
		return formatStringImpl(boostFormat, std::forward<Arguments>(args)...);
	}
	template<typename... Arguments>
	inline std::string formatString(const char* const format, Arguments&&... args)
	{
		boost::format boostFormat(format);
		return formatStringImpl(boostFormat, std::forward<Arguments>(args)...);
	}

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
		archive.write("writeBuffer", writeBuffer);
		return archive;
	}

	template<typename SizeType = uint32_t>
	bool readFromArchive(const Archive& archive, std::string& string)
	{
		static_assert(std::is_integral<SizeType>::value, "SizeType must be integral.");
		WriteBuffer writeBuffer;
		archive.read("writeBuffer", writeBuffer);
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

	bool doesStartWith(const std::string_view string, const std::string_view searchParameter);
	bool doesEndWith(const std::string_view string, const std::string_view searchParameter);
	bool doesContain(const std::string_view string, const std::string_view searchParameter);

	/*
		Adds 'indentation' at the start of the string and after each new line character in the string, with the following expection:
			-if the last character of the string is a new line character, no indentation is added after it.
	*/
	void indent(std::string& string, const std::string_view indentation);

	/*
		Returns a nicely formated time length string.
		Presicion: how many digits to display.
	*/
	std::string toTimeLengthString(const time::Time& time, const size_t precision);

	/*
		Returns a nicely formated byte size string.
		Cuts off some of the precision.
		Adds a unit suffix (B/KB/MB/GB).
	*/
	std::string toByteString(const uint64_t bytes);

	/*
		Multiplier is expected to be greater or equal to 0.
		0.9f -> "-10 %"
		1.0f -> "+0 %"
		1.1f -> "+10 %"
	*/
	std::string toMultiplierPercentageString(const float multiplier, const size_t precision);

	bool fromString(const std::string_view string, int64_t& valueOut);
	bool fromString(const std::string_view string, int32_t& valueOut);
	bool fromString(const std::string_view string, int16_t& valueOut);
	bool fromString(const std::string_view string, int8_t& valueOut);
	bool fromString(const std::string_view string, uint64_t& valueOut);
	bool fromString(const std::string_view string, uint32_t& valueOut);
	bool fromString(const std::string_view string, uint16_t& valueOut);
	bool fromString(const std::string_view string, uint8_t& valueOut);
	bool fromString(const std::string_view string, float& valueOut);
	bool fromString(const std::string_view string, double& valueOut);

	std::string toLowerCase(const std::string_view string);
	std::string toUpperCase(const std::string_view string);

	std::wstring toWideString(const std::string& string);
	std::string fromWideString(const std::wstring& wstring);
}
