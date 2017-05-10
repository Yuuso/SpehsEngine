#pragma once
#include <string>
#include <stdint.h>


namespace spehs
{
	/**Returns string as float. Returns 0.0f if string is invalid*/
	float getStringAsFloat(const std::string& string);

	/**Returns string as integer. Returns 0 if string is invalid*/
	int getStringAsInt(const std::string& string);

	/**Extended std::to_string() function. Crops out extra precision given by std::to_string(). Maximum precision 6!*/
	std::string toString(const float value, const int precision);

	/**Replaces some spaces in the string with newline characters so that no row exceeds given maximum row width in character count, if possible.*/
	void arrangeIntoRows(std::string& string, const int maxRowWidth);

	/**Returns a string representing the integer in hex format. NOTES: 1: no '0x'-prefix will be given, 2: all zero bits will be filled in and represented in '0' characters*/
	std::string toHexString(const uint8_t integer);
	std::string toHexString(const int8_t integer);
	std::string toHexString(const uint16_t integer);
	std::string toHexString(const int16_t integer);
	std::string toHexString(const uint32_t integer);
	std::string toHexString(const int32_t integer);
	std::string toHexString(const uint64_t integer);
	std::string toHexString(const int64_t integer);

	/**Returns seconds expressed in 'stopwatch notation' string. Format: hh:mm:ss */
	std::string secondsToStopwatchNotation(int seconds);
}