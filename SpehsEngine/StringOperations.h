#pragma once
#include <string>


namespace SpehsEngine
{
	///<Returns string as float. Returns 0.0f if string is invalid
	float getStringAsFloat(const std::string& string);

	///<Returns string as float. Returns 0.0f if string is invalid
	int getStringAsInt(const std::string& string);
}