#pragma once
#include <string>
#include <vector>
#include <stdint.h>


namespace se
{
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

	// string_view + operators
	std::string operator+(const std::string& _string, const std::string_view _string_view);
	std::string operator+(const std::string_view _string_view, const std::string& _string);
	void operator+=(std::string& _string, const std::string_view _string_view);
}