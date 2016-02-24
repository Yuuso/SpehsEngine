#include <stdint.h>
#include "BitwiseOperations.h"
#include "StringOperations.h"

namespace SpehsEngine
{
	float getStringAsFloat(const std::string& string)
	{
		int8_t stringState = 0;
		int intValue = 0;
		float floatValue = 0.0f;
		for (unsigned i = 0; i < string.size(); i++)
		{
			if (string[i] >= '0' && string[i] <= '9')
			{//Add numerical value
				if (checkBit(stringState, 2))
				{//Decimal
					floatValue *= 0.1f;
					floatValue += int(string[i] - 48) * 0.1f;
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
}