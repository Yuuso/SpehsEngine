#pragma once
#include <string>
#include <vector>
#include <stdint.h>


namespace se
{
	/**Returns string as float. Returns 0.0f if string is invalid*/
	float getStringAsFloat(const std::string& string);

	/**Returns string as integer. Returns 0 if string is invalid*/
	int getStringAsInt(const std::string& string);

	/** Returns "true"/"false". */
	std::string toString(const bool value);

	/**Extended std::to_string() function. Crops out extra precision given by std::to_string(). Maximum precision 6!*/
	std::string toString(const float value, const int precision);

	/*If value is larger than zero, adds the '+' character prefix into the string.*/
	template<typename T>
	typename std::enable_if<std::is_integral<T>::value, std::string>::type toSignedString(const T value)
	{
		if (value > 0)
			return "+" + std::to_string(value);
		else
			return std::to_string(value);
	}
	std::string toSignedString(const float value, const int precision);

	/**Returns a string representing the integer in hex format. NOTES: 1: no '0x'-prefix will be given, 2: all zero bits will be filled in and represented in '0' characters*/
	std::string toHexString(const uint8_t integer);
	std::string toHexString(const uint16_t integer);
	std::string toHexString(const uint32_t integer);
	std::string toHexString(const uint64_t integer);

	/**Returns seconds expressed in 'stopwatch notation' string. Format: hh:mm:ss */
	std::string secondsToStopwatchNotation(int seconds);

	/**Returns IPv4 address given in string format as an unsigned integer*/
	unsigned getStringAsIPv4Address(const std::string& string);

	/**Replaces some spaces in the string with newline characters so that no row exceeds given maximum row width in character count, if possible.*/
	void arrangeIntoRows(std::string& string, const int maxRowWidth);

	/** Process string into words (character sequences that are separated by the ' ' ascii character). Words are appended to the vector. */
	void getWords(const std::string& string, std::vector<std::string>& wordsDeposit);
}