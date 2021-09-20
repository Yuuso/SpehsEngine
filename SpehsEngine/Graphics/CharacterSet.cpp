#include "stdafx.h"
#include "SpehsEngine/Graphics/CharacterSet.h"

#include <codecvt>


namespace se
{
	namespace graphics
	{
		static_assert(std::is_same<CharacterCode, std::u32string_view::traits_type::char_type>::value, "CharacterCode type mismatch!");

		void collectCharactersFromString(CharacterSet& _set, const std::string& _string)
		{
			std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
			return collectCharactersFromString(_set, converter.from_bytes(_string));
		}
		void collectCharactersFromString(CharacterSet& _set, const std::u32string_view _string)
		{
			_set.insert(_string.begin(), _string.end());
		}
	}
}
