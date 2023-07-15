#pragma once

#include <string>


namespace ImGui
{
	// Pointer to null terminated, not owned char array.
	struct Label
	{
		constexpr Label(const char* const _string) : string(_string) {}
		Label(const std::string& _string) : string(_string.c_str()) {}
		constexpr operator const char* () const { return string; }
		const char* const string = nullptr;
	};
}