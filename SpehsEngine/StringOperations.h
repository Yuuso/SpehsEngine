#pragma once
#include <string>


namespace spehs
{
	/**Returns string as float. Returns 0.0f if string is invalid*/
	float getStringAsFloat(const std::string& string);

	/**Returns string as float. Returns 0.0f if string is invalid*/
	int getStringAsInt(const std::string& string);

	/**Extended std::to_string() function. Crops out extra precision given by std::to_string(). Maximum precision 6!*/
	std::string toString(float value, int precision);
}