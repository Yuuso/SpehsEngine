#include "stdafx.h"
#include <string>
#include <stdint.h>
#include "SpehsEngine/Core/BitwiseOperations.h"
#include "SpehsEngine/Core/StringOperations.h"

namespace se
{
	float getStringAsFloat(const std::string& string)
	{
		int stringState = 0;
		int intValue = 0;
		float floatValue = 0.0f;
		float decimalFactor = 0.1f;
		for (unsigned i = 0; i < string.size(); i++)
		{
			if (string[i] >= '0' && string[i] <= '9')
			{//Add numerical value
				if (checkBit(stringState, 2))
				{//Decimal
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
				{//Second '-' character, return
					return 0.0f;
				}
				enableBit(stringState, 1);//Negative
			}
			else if (string[i] == '.' || string[i] == ',')
			{
				if (checkBit(stringState, 2))
				{//Second ',/.' character, return
					return 0.0f;
				}
				enableBit(stringState, 2);//Begin decimal part
			}
			else
			{//Character is unknown
				return 0.0f;
			}
		}

		//Add up values, negate if needed
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
			{//Add numerical value
				{
					intValue *= 10;
					intValue += int(string[i] - 48);
				}
			}
			else if (string[i] == '-')
			{
				if (checkBit(stringState, 1))
				{//Second '-' character, return
					return 0;
				}
				enableBit(stringState, 1);//Negative
			}
			else if (string[i] == '.' || string[i] == ',')
			{
				if (checkBit(stringState, 2))
				{//Second ',/.' character, return
					return 0;
				}
				enableBit(stringState, 2);//Begin decimal part
			}
			else
			{//Character is unknown
				return 0;
			}
		}

		//negate if needed
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
}