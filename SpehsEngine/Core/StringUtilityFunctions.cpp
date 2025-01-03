#include "stdafx.h"
#include "SpehsEngine/Core/StringUtilityFunctions.h"

#include "boost/lexical_cast.hpp"

#if SE_PLATFORM == SE_PLATFORM_WINDOWS
#include <Windows.h>
#endif


namespace se
{
	bool doesStartWith(const std::string_view string, const std::string_view searchParameter)
	{
		if (searchParameter.size() > string.size())
		{
			return false;
		}
		else
		{
			for (size_t i = 0; i < searchParameter.size(); i++)
			{
				if (string[i] != searchParameter[i])
				{
					return false;
				}
			}
			return true;
		}
	}

	bool doesEndWith(const std::string_view string, const std::string_view searchParameter)
	{
		if (searchParameter.size() > string.size())
		{
			return false;
		}
		else
		{
			const size_t offset = string.size() - searchParameter.size();
			for (size_t i = 0; i < searchParameter.size(); i++)
			{
				if (string[offset + i] != searchParameter[i])
				{
					return false;
				}
			}
			return true;
		}
	}

	bool doesContain(const std::string_view string, const std::string_view searchParameter)
	{
		return string.find(searchParameter) != std::string::npos;
	}

	void indent(std::string& string, const std::string_view indentation)
	{
		if (string.empty())
		{
			return;
		}

		string.insert(string.begin(), indentation.begin(), indentation.end());

		for (size_t i = indentation.size(); i < string.size() - 1; i++)
		{
			if (string[i] == '\n')
			{
				string.insert(string.begin() + i + 1, indentation.begin(), indentation.end());
				i += indentation.size();
			}
		}
	}

	std::string toTimeLengthString(const Time& time, const size_t precision)
	{
		// TODO: proper precision
		if (time.value >= detail::time_conversion_rate::second)
		{
			const float f = time.asSeconds();
			return toString(f, precision) + " s";
		}
		else if (time.value >= detail::time_conversion_rate::millisecond)
		{
			const float f = time.asMilliseconds();
			return toString(f, precision) + " ms";
		}
		else if (time.value >= detail::time_conversion_rate::microsecond)
		{
			const float f = time.asMicroseconds();
			return toString(f, precision) + " us";
		}
		else
		{
			const float f = time.asNanoseconds();
			return toString(f, precision) + " ns";
		}
	}

	std::string toByteString(const uint64_t bytes)
	{
		if (bytes >= 1024ull * 1024ull * 1024ull * 1024ull * 1024ull)
		{
			return toString(float(double(bytes) / double(1024ull * 1024ull * 1024ull * 1024ull * 1024ull)), 2) + " PB";
		}
		else if (bytes >= 1024ull * 1024ull * 1024ull * 1024ull)
		{
			return toString(float(double(bytes) / double(1024ull * 1024ull * 1024ull * 1024ull)), 2) + " TB";
		}
		else if (bytes >= 1024ull * 1024ull * 1024ull)
		{
			return toString(float(double(bytes) / double(1024ull * 1024ull * 1024ull)), 2) + " GB";
		}
		else if (bytes >= 1024ull * 1024ull)
		{
			return toString(float(double(bytes) / double(1024ull * 1024ull)), 2) + " MB";
		}
		else if (bytes >= 1024ull)
		{
			return toString(float(double(bytes) / double(1024ull)), 2) + " KB";
		}
		else
		{
			return std::to_string(bytes) + " B";
		}
	}

	std::string toMultiplierPercentageString(const float multiplier, const size_t precision)
	{
		se_assert(multiplier >= 0.0f);
		return toSignedString((multiplier - 1.0f) * 100.0f, precision) + "%";
	}

	template<typename T>
	inline bool fromStringImpl(const std::string_view string, T& valueOut)
	{
		try
		{
			valueOut = boost::lexical_cast<T>(string);
			return true;
		}
		catch (const std::exception&)
		{
			valueOut = 0;
			return false;
		}
	}

	bool fromString(const std::string_view string, int64_t& valueOut)
	{
		return fromStringImpl(string, valueOut);
	}

	bool fromString(const std::string_view string, int32_t& valueOut)
	{
		return fromStringImpl(string, valueOut);
	}

	bool fromString(const std::string_view string, int16_t& valueOut)
	{
		return fromStringImpl(string, valueOut);
	}

	bool fromString(const std::string_view string, int8_t& valueOut)
	{
		return fromStringImpl(string, valueOut);
	}

	bool fromString(const std::string_view string, uint64_t& valueOut)
	{
		return fromStringImpl(string, valueOut);
	}

	bool fromString(const std::string_view string, uint32_t& valueOut)
	{
		return fromStringImpl(string, valueOut);
	}

	bool fromString(const std::string_view string, uint16_t& valueOut)
	{
		return fromStringImpl(string, valueOut);
	}

	bool fromString(const std::string_view string, uint8_t& valueOut)
	{
		return fromStringImpl(string, valueOut);
	}

	bool fromString(const std::string_view string, float& valueOut)
	{
		return fromStringImpl(string, valueOut);
	}

	bool fromString(const std::string_view string, double& valueOut)
	{
		return fromStringImpl(string, valueOut);
	}

	std::string toLowerCase(const std::string_view string)
	{
		std::string s(string);
		for (char& c : s)
		{
			c = char(std::tolower(int(c)));
		}
		return s;
	}

	std::string toUpperCase(const std::string_view string)
	{
		std::string s(string);
		for (char& c : s)
		{
			c = char(std::toupper(int(c)));
		}
		return s;
	}

	std::wstring toWideString(const std::string& string)
	{
		std::wstring wstring;

#if SE_PLATFORM == SE_PLATFORM_WINDOWS
		if (!string.empty())
		{
			const size_t requiredLength = ::MultiByteToWideChar(CP_UTF8, 0, string.c_str(), (int)string.length(), 0, 0);
			wstring = std::wstring(requiredLength, L'\0');
			::MultiByteToWideChar(CP_UTF8, 0, string.c_str(), (int)string.length(), &wstring[0], (int)wstring.length());
		}
#else
		static_assert(false, "toWideString() implementation missing");
#endif
		return wstring;
	}

	std::string fromWideString(const std::wstring& wstring)
	{
		std::string string;
#if SE_PLATFORM == SE_PLATFORM_WINDOWS
		if (!wstring.empty())
		{
			LPCCH defaultChar = NULL;
			LPBOOL usedDefaultChar = NULL;
			const size_t requiredLength = ::WideCharToMultiByte(CP_UTF8, 0, wstring.c_str(), (int)wstring.length(), 0, 0, defaultChar, usedDefaultChar);
			string = std::string(requiredLength, '\0');
			::WideCharToMultiByte(CP_UTF8, 0, wstring.c_str(), (int)wstring.length(), &string[0], (int)string.length(), defaultChar, usedDefaultChar);
		}
#else
		static_assert(false, "toWideString() implementation missing");
#endif
		return string;
	}

	std::string variableNameToDisplay(const std::string_view optionName, const bool startWithUpperCase)
	{
		std::string result;
		result.reserve(optionName.length());
		bool wasUpper = false;
		for (size_t i = 0; i < optionName.length(); i++)
		{
			char c = optionName[i];
			const bool isUpper = std::isupper(c);
			const bool hasNext = i + 1 < optionName.length();
			const bool nextIsUpper = hasNext ? std::isupper(optionName[i + 1]) : true;
			if (i > 0)
			{
				if ((isUpper && !wasUpper) || (wasUpper && isUpper && !nextIsUpper))
				{
					result += " ";
					if (!nextIsUpper)
					{
						// New words start with lower case (with expection of words that consist of multiple capital letters in a sequence)
						c = char(std::tolower(c));
					}
				}
			}
			else if (startWithUpperCase)
			{
				c = char(std::toupper(c));
			}
			else if (isUpper && nextIsUpper)
			{
				// All upper case acronym, do not change to lower
			}
			else if (startWithUpperCase)
			{
				c = char(std::tolower(c));
			}
			result += c;
			wasUpper = isUpper;
		}
		return result;
	}

	float getStringAsFloat(const std::string& string)
	{
		int stringState = 0;
		int intValue = 0;
		float floatValue = 0.0f;
		float decimalFactor = 0.1f;
		for (unsigned i = 0; i < string.size(); i++)
		{
			if (string[i] >= '0' && string[i] <= '9')
			{
				// Add numerical value
				if (checkBit(stringState, 2))
				{
					// Decimal
					floatValue += int(string[i] - 48) * decimalFactor;
					decimalFactor *= 0.1f;
				}
				else
				{
					intValue *= 10;
					intValue += int(string[i] - 48);
				}
			}
			else if (string[i] == '-')
			{
				if (checkBit(stringState, 1))
				{
					// Second '-' character, return
					return 0.0f;
				}
				enableBit(stringState, 1); // Negative
			}
			else if (string[i] == '.' || string[i] == ',')
			{
				if (checkBit(stringState, 2))
				{
					// Second ',/.' character, return
					return 0.0f;
				}
				enableBit(stringState, 2); // Begin decimal part
			}
			else
			{
				// Character is unknown
				return 0.0f;
			}
		}

		// Add up values, negate if needed
		floatValue += intValue;
		if (checkBit(stringState, 1))
			floatValue *= -1;

		return floatValue;
	}

	int getStringAsInt(const std::string& string)
	{
		int stringState = 0;
		int intValue = 0;
		for (unsigned i = 0; i < string.size(); i++)
		{
			if (string[i] >= '0' && string[i] <= '9')
			{
				// Add numerical value
				{
					intValue *= 10;
					intValue += int(string[i] - 48);
				}
			}
			else if (string[i] == '-')
			{
				if (checkBit(stringState, 1))
				{
					// Second '-' character, return
					return 0;
				}
				enableBit(stringState, 1); // Negative
			}
			else if (string[i] == '.' || string[i] == ',')
			{
				if (checkBit(stringState, 2))
				{
					// Second ',/.' character, return
					return 0;
				}
				enableBit(stringState, 2); // Begin decimal part
			}
			else
			{
				// Character is unknown
				return 0;
			}
		}

		// Negate if needed
		if (checkBit(stringState, 1))
			intValue *= -1;

		return intValue;
	}

	std::string toString(const bool value)
	{
		if (value)
		{
			return "true";
		}
		else
		{
			return "false";
		}
	}

	std::string toString(const float value, const size_t precision)
	{
#ifdef _DEBUG
		if (precision > 6)	log::error("se::toString() precision too high! Maximum precision 6!");
#endif
		std::string str(std::to_string(value));
		for (size_t i = 0; i < str.size(); i++)
		{
			if (str[i] == '.')
			{
				if (precision == 0)
				{
					str.resize(i);
				}
				else
				{
					str.resize(i + 1 + precision);
				}
				return str;
			}
		}
		return std::string("");
	}

	std::string toSignedString(const float value, const size_t precision)
	{
		if (value > 0)
			return "+" + toString(value, precision);
		else
			return toString(value, precision);
	}

	static const char hexTable[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	std::string toHexString(const uint8_t integer)
	{
		std::string str(2, '0');
		str[0] = hexTable[integer >> 4];
		str[1] = hexTable[uint8_t(integer << 4) >> 4];
		return str;
	}
	std::string toHexString(const uint16_t integer)
	{
		std::string str(4, '0');
		str[0] = hexTable[integer >> 12];
		str[1] = hexTable[uint16_t(integer << 4) >> 12];
		str[2] = hexTable[uint16_t(integer << 8) >> 12];
		str[3] = hexTable[uint16_t(integer << 12) >> 12];
		return str;
	}
	std::string toHexString(const uint32_t integer)
	{
		std::string str(8, '0');
		str[0] = hexTable[integer >> 28];
		str[1] = hexTable[uint32_t(integer << 4) >> 28];
		str[2] = hexTable[uint32_t(integer << 8) >> 28];
		str[3] = hexTable[uint32_t(integer << 12) >> 28];
		str[4] = hexTable[uint32_t(integer << 16) >> 28];
		str[5] = hexTable[uint32_t(integer << 20) >> 28];
		str[6] = hexTable[uint32_t(integer << 24) >> 28];
		str[7] = hexTable[uint32_t(integer << 28) >> 28];
		return str;
	}
	std::string toHexString(const uint64_t integer)
	{
		std::string str(16, '0');
		str[0] = hexTable[integer >> 60];
		str[1] = hexTable[(integer << 4) >> 60];
		str[2] = hexTable[(integer << 8) >> 60];
		str[3] = hexTable[(integer << 12) >> 60];
		str[4] = hexTable[(integer << 16) >> 60];
		str[5] = hexTable[(integer << 20) >> 60];
		str[6] = hexTable[(integer << 24) >> 60];
		str[7] = hexTable[(integer << 28) >> 60];
		str[8] = hexTable[(integer << 32) >> 60];
		str[9] = hexTable[(integer << 36) >> 60];
		str[10] = hexTable[(integer << 40) >> 60];
		str[11] = hexTable[(integer << 44) >> 60];
		str[12] = hexTable[(integer << 48) >> 60];
		str[13] = hexTable[(integer << 52) >> 60];
		str[14] = hexTable[(integer << 56) >> 60];
		str[15] = hexTable[(integer << 60) >> 60];
		return str;
	}
	std::string secondsToStopwatchNotation(const int seconds)
	{
		int absSeconds = std::abs(seconds);
		int minutes = absSeconds / 60;
		absSeconds -= minutes * 60;
		int hours = minutes / 60;
		minutes -= hours * 60;
		return std::string(seconds < 0 ? "-" : "") +
			(hours < 10 ? "0" : "") + std::to_string(hours) + ":" +
			(minutes < 10 ? "0" : "") + std::to_string(minutes) + ":" +
			(absSeconds < 10 ? "0" : "") + std::to_string(absSeconds);
	}

	unsigned getStringAsIPv4Address(const std::string& string)
	{
		unsigned address = 0;
		int byteIndex = 0;
		std::string currentByteString;
		for (unsigned i = 0; i < string.size(); i++)
		{
			if (string[i] == '.')
			{//Next byte
				const int byteToInt = std::atoi(currentByteString.c_str());
				if (byteToInt < 0 || byteToInt > 255)
				{//Invalid "byte"
					log::warning("getStringAsIPv4Address() failed! A byte value is too big (larger than 255)!");
					return 0;
				}
				address |= (unsigned char)byteToInt << (24 - 8 * byteIndex++);
				currentByteString.clear();
			}
			else if (string[i] >= 48 && string[i] <= 57)
			{//Append current byte
				currentByteString += string[i];
			}
			else
			{
				log::warning("getStringAsIPv4Address() failed! Address contains invalid characters!");
				return 0;
			}
		}

		//Append last byte...
		const int byteToInt = std::atoi(currentByteString.c_str());
		if (byteToInt < 0 || byteToInt > 255)
		{//Invalid "byte"
			log::warning("getStringAsIPv4Address() failed! A byte value is too big (larger than 255)!");
			return 0;
		}
		address |= (unsigned char)byteToInt << (24 - 8 * byteIndex++);

		return address;
	}

	void arrangeIntoRows(std::string& string, const int maxRowWidth)
	{
		int lastNewLineIndex(0);
		int lastSpaceIndex(0);
		int width(0);
		for (unsigned i = 0; i < string.size(); i++)
		{
			if (string[i] == ' ')
				lastSpaceIndex = i;

			if (string[i] == '\n')
			{//Newline
				lastNewLineIndex = i;
				width = 0;
			}
			else if (++width > maxRowWidth)
			{//Exceeds row width, look for the last space index

				if (lastSpaceIndex > lastNewLineIndex)
				{//Space exists, set newline there
					string[lastSpaceIndex] = '\n';
					lastNewLineIndex = lastSpaceIndex;
					width = i - lastSpaceIndex;
				}
				else if (string[i] == ' ')
				{//Make a newline here
					string[i] = '\n';
					lastNewLineIndex = i;
					width = 0;
				}
			}
		}
	}

	void getWords(const std::string& string, std::vector<std::string>& words)
	{
		size_t wordBeginIndex = 0;
		const size_t lastIndex = string.size() - 1;
		for (size_t i = 0; ; i++)
		{
			if (string[i] == ' ')
			{
				const size_t wordLength = i - wordBeginIndex;
				if (wordLength > 0)
					words.push_back(std::string(string.begin() + wordBeginIndex, string.begin() + i));
				wordBeginIndex = i + 1;
			}
			if (i == lastIndex)
			{
				const size_t wordLength = i + 1 - wordBeginIndex;
				if (wordLength > 0)
					words.push_back(std::string(string.begin() + wordBeginIndex, string.begin() + i + 1));
				return;
			}
		}
	}

	bool getFileExtension(const std::string& inString, std::string& outString)
	{
		for (size_t i = inString.size(); i-- > 0;)
		{
			if (inString[i] == '.')
			{
				outString.clear();
				if (i + 1 < inString.size())
				{
					outString.insert(outString.begin(), inString.begin() + i + 1, inString.end());
				}
				return true;
			}
		}
		return false;
	}

	size_t findAndReplaceAll(std::string& string, const std::string_view v1, const std::string_view v2)
	{
		size_t count = 0;
		while (true)
		{
			const size_t start = string.find(v1);
			if (start == std::string::npos)
				break;
			string.replace(start, v1.length(), v2);
			count++;
		}
		return count;
	}
	size_t findAndReplaceAll(std::wstring& string, const std::wstring_view v1, const std::wstring_view v2)
	{
		size_t count = 0;
		while (true)
		{
			const size_t start = string.find(v1);
			if (start == std::wstring::npos)
				break;
			string.replace(start, v1.length(), v2);
			count++;
		}
		return count;
	}
}
