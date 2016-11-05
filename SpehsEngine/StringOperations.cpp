#include <string>
#include <stdint.h>
#include "Console.h"
#include "BitwiseOperations.h"
#include "StringOperations.h"

namespace spehs
{
	float getStringAsFloat(const std::string& string)
	{
		int8_t stringState = 0;
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
		int8_t stringState = 0;
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

		//negate if needed
		if (checkBit(stringState, 1))
			intValue *= -1;

		return intValue;
	}

	std::string toString(float value, int precision)
	{
#ifdef _DEBUG
		if (precision > 6)	spehs::console::error(__FUNCTION__" precision too high! Maximum precision 6!");
#endif
		std::string str(std::to_string(value));
		for (unsigned i = 0; i < str.size(); i++)
		{
			if (str[i] == '.')
			{
				str.resize(i + 1 + precision);
				return str;
			}
		}
	}

	void arrangeIntoRows(std::string& string, int maxRowWidth)
	{
		int lastNewLineIndex(0);
		int lastSpaceIndex(0);
		int width(0);
		for (unsigned i = 0; i < string.size(); i++)
		{
			if (string[i] == ' ')
				lastSpaceIndex = i;

			if (++width > maxRowWidth)
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

	char hexTable[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	std::string toHexString(uint8_t integer)
	{
		std::string str(2, '0');
		str[0] = hexTable[integer >> 4];
		str[1] = hexTable[(integer << 4) >> 4];
		return str;
	}
	std::string toHexString(int8_t integer)
	{
		std::string str(2, '0');
		str[0] = hexTable[integer >> 4];
		str[1] = hexTable[(integer << 4) >> 4];
		return str;
	}
	std::string toHexString(uint16_t integer)
	{
		std::string str(4, '0');
		str[0] = hexTable[integer >> 12];
		str[1] = hexTable[(integer << 4) >> 12];
		str[2] = hexTable[(integer << 8) >> 12];
		str[3] = hexTable[(integer << 12) >> 12];
		return str;
	}
	std::string toHexString(int16_t integer)
	{
		std::string str(4, '0');
		str[0] = hexTable[integer >> 12];
		str[1] = hexTable[(integer << 4) >> 12];
		str[2] = hexTable[(integer << 8) >> 12];
		str[3] = hexTable[(integer << 12) >> 12];
		return str;
	}
	std::string toHexString(uint32_t integer)
	{
		std::string str(8, '0');
		str[0] = hexTable[integer >> 28];
		str[1] = hexTable[(integer << 4) >> 28];
		str[2] = hexTable[(integer << 8) >> 28];
		str[3] = hexTable[(integer << 12) >> 28];
		str[4] = hexTable[(integer << 16) >> 28];
		str[5] = hexTable[(integer << 20) >> 28];
		str[6] = hexTable[(integer << 24) >> 28];
		str[7] = hexTable[(integer << 28) >> 28];
		return str;
	}
	std::string toHexString(int32_t integer)
	{
		std::string str(8, '0');
		str[0] = hexTable[integer >> 28];
		str[1] = hexTable[(integer << 4) >> 28];
		str[2] = hexTable[(integer << 8) >> 28];
		str[3] = hexTable[(integer << 12) >> 28];
		str[4] = hexTable[(integer << 16) >> 28];
		str[5] = hexTable[(integer << 20) >> 28];
		str[6] = hexTable[(integer << 24) >> 28];
		str[7] = hexTable[(integer << 28) >> 28];
		return str;
	}
	std::string toHexString(uint64_t integer)
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
	std::string toHexString(int64_t integer)
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
}