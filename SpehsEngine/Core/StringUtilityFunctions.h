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

	template<> template<typename S, typename T>
	static bool se::Serial<std::string>::impl(S& _serial, T _string)
	{
		typedef typename se::remove_cvref<T>::type VectorType;
		typedef typename VectorType::value_type ValueType;
		if constexpr (S::getWritingEnabled())
		{
			const ByteView byteView((const std::byte*)_string.c_str(), _string.length());
			se_serial(_serial, byteView, "string");
			return true;
		}
		else
		{
			ByteVector byteVector;
			se_serial(_serial, byteVector, "string");
			_string.resize(byteVector.getSize());
			memcpy(_string.data(), (const void*)byteVector.getData(), byteVector.getSize());
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

	/*
		Converts variable/class name into a format better suitable for logging or showing in GUI.
		Names are expected to follow the camelCase convention, with a few exceptions explained below for all caps acronyms.
		---
		INPUT				START UPPER: true		START UPPER: false
		TwoWords			Two words				two words
		twoWords			Two words				two words
		SecondACRONYM		Second ACRONYM			second ACRONYM
		ACRONYMSecond		ACRONYM second			ACRONYM second
	*/
	std::string variableNameToDisplay(const std::string_view string, const bool startWithUpperCase);

	// Returns string as float. Returns 0.0f if string is invalid
	float getStringAsFloat(const std::string& string);

	// Returns string as integer. Returns 0 if string is invalid
	int getStringAsInt(const std::string& string);

	// Returns "true" or "false"
	std::string toString(const bool value);

	// Extended std::to_string() function. Crops out extra precision given by std::to_string(). Maximum precision 6.
	std::string toString(const float value, const size_t precision);

	// If value is larger than zero, adds the '+' character prefix into the string
	template<typename T>
	typename std::enable_if<std::is_integral<T>::value, std::string>::type toSignedString(const T value)
	{
		if (value > 0)
			return "+" + std::to_string(value);
		else
			return std::to_string(value);
	}
	std::string toSignedString(const float value, const size_t precision);

	/*
		Returns a string representing the integer in hex format.
			- no '0x'-prefix
			- all zero bits will be filled in and represented in '0' characters
	*/
	std::string toHexString(const uint8_t integer);
	std::string toHexString(const uint16_t integer);
	std::string toHexString(const uint32_t integer);
	std::string toHexString(const uint64_t integer);

	// Returns seconds expressed in 'stopwatch notation' string. Format: hh:mm:ss
	std::string secondsToStopwatchNotation(int seconds);

	// Returns IPv4 address given in string format as an unsigned integer
	unsigned getStringAsIPv4Address(const std::string& string);

	// Replaces some spaces in the string with newline characters so that no row exceeds given maximum row width in character count, if possible
	void arrangeIntoRows(std::string& string, const int maxRowWidth);

	// Process string into words (character sequences that are separated by the ' ' ascii character)
	void getWords(const std::string& string, std::vector<std::string>& wordsDeposit);

	/*
		Iterates the input string from back to front. If dot character is encountered, returns the contents of that string after the first.
		Assigns the contents after the last dot character in the inString into the outString. If no dot character exists, returns false.
	*/
	bool getFileExtension(const std::string& inString, std::string& outString);

	// Iterate string and replace all instances of 'v1' with 'v2'. Returns the number of replacements performed.
	size_t findAndReplaceAll(std::string& string, const std::string_view v1, const std::string_view v2);
	size_t findAndReplaceAll(std::wstring& string, const std::wstring_view v1, const std::wstring_view v2);
}
