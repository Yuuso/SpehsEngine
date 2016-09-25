#pragma once
#include <string>
#include <stdint.h>


namespace spehs
{
	/**Returns string as float. Returns 0.0f if string is invalid*/
	float getStringAsFloat(const std::string& string);

	/**Returns string as float. Returns 0.0f if string is invalid*/
	int getStringAsInt(const std::string& string);

	/**Extended std::to_string() function. Crops out extra precision given by std::to_string(). Maximum precision 6!*/
	std::string toString(float value, int precision);

	/**Returns a string representing the integer in hex format. NOTES: 1: no '0x'-prefix will be given, 2: all zero bits will be filled in and represented in '0' characters*/
	std::string toHexString(uint8_t integer);
	std::string toHexString(int8_t integer);
	std::string toHexString(uint16_t integer);
	std::string toHexString(int16_t integer);
	std::string toHexString(uint32_t integer);
	std::string toHexString(int32_t integer);
	std::string toHexString(uint64_t integer);
	std::string toHexString(int64_t integer);
}