#pragma once


namespace ImGui
{
	// Pointer to a null terminated, not owned char array.
	struct String
	{
		constexpr String(const char* const _string) : pointer(_string) {}
		String(const std::string& _string) : pointer(_string.c_str()) {}
		constexpr operator const char* () const { return pointer; }
		constexpr explicit operator bool () const { return pointer != nullptr; }
		const char* const pointer = nullptr;
	};
}