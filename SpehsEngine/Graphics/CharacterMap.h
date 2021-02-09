#pragma once

#include <vector>


namespace se
{
	namespace graphics
	{
		typedef char32_t CharacterCode;
		typedef std::vector<CharacterCode> CharacterMap;

		constexpr CharacterCode replacementCharacter = U'\xFFFD';

		static const CharacterMap defaultCharacterMap = {
			// Basic ASCII Map
																						   U'!', U'"', U'#', U'$', U'%', U'&', U'\'',
			U'(', U')', U'*', U'+', U',', U'-', U'.', U'/', U'0', U'1', U'2', U'3', U'4',  U'5', U'6', U'7', U'8', U'9', U':', U';',
			U'<', U'=', U'>', U'?', U'@', U'A', U'B', U'C', U'D', U'E', U'F', U'G', U'H',  U'I', U'J', U'K', U'L', U'M', U'N', U'O',
			U'P', U'Q', U'R', U'S', U'T', U'U', U'V', U'W', U'X', U'Y', U'Z', U'[', U'\\', U']', U'^', U'_', U'`', U'a', U'b', U'c',
			U'd', U'e', U'f', U'g', U'h', U'i', U'j', U'k', U'l', U'm', U'n', U'o', U'p',  U'q', U'r', U's', U't', U'u', U'v', U'w',
			U'x', U'y', U'z', U'{', U'|', U'}', U'~', U' '
		};
	}
}
