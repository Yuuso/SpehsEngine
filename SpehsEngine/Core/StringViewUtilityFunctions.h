#pragma once

#include <string>


// string_view + operators

inline std::string operator+(const std::string& _string, const std::string_view _string_view)
{
	return _string + std::string(_string_view);
}
inline std::string operator+(const std::string_view _string_view, const std::string& _string)
{
	return std::string(_string_view) + _string;
}
inline void operator+=(std::string& _string, const std::string_view _string_view)
{
	_string += std::string(_string_view);
}