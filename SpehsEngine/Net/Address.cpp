#include "stdafx.h"
#include "SpehsEngine/Net/Address.h"

#include "SpehsEngine/Core/StringUtilityFunctions.h"


namespace se
{
	namespace net
	{
		const Address Address::invalid = Address();

		Address Address::makeFromStringIpv4(const std::string_view& _string)
		{
			uint32_t value = 0;
			uint8_t* const valuePtr8 = (unsigned char*)(&value);
			size_t byteIndex = 3;
			size_t numberBeginIndex = 0;
			for (size_t i = 0; i < _string.size() + 1; i++)
			{
				if (i == _string.size() || _string[i] == '.')
				{
					const size_t numberLength = i - numberBeginIndex;
					if (numberLength > 0 && numberLength < 4)
					{
						int number = 0;
						const std::string_view numberString(_string.data() + numberBeginIndex, numberLength);
						if (fromString(numberString, number))
						{
							if (number < 0 || number > 255)
							{
								return invalid;
							}
							const uint8_t byte = uint8_t(number);
							memcpy(valuePtr8 + byteIndex, &byte, sizeof(byte));
							if (byteIndex-- > 0)
							{
								numberBeginIndex = i + 1;
							}
							else
							{
								break;
							}
						}
						else
						{
							return invalid;
						}
					}
					else
					{
						return invalid;
					}
				}
			}
			return Address(value);
		}

		std::string Address::toString() const
		{
			std::string string;
			switch (type)
			{
			case Type::None:
				string = "Invalid";
				break;
			case Type::Ipv4:
				for (size_t i = 4; i-- > 0;)
				{
					uint8_t byte = 0;
					memcpy(&byte, (const uint8_t*)&data0 + i, sizeof(byte));
					string += std::to_string(byte);
					if (i > 0)
					{
						string += ".";
					}
				}
				break;
			case Type::Ipv6:
				for (size_t a = 0; a < 8; a++)
				{
					if (a > 0)
					{
						string += ":";
					}
					for (size_t b = 0; b < 2; b++)
					{
						const size_t i = a * 8 + b;
						uint8_t byte = 0;
						memcpy(&byte, (const uint8_t*)&data0 + i, sizeof(byte));
						string += toHexString(byte);
					}
				}
				break;
			}
			return string;
		}
	}
}
