#pragma once

#include "SpehsEngine/Core/Serial/Serial.h"
#include <stdint.h>
#include <optional>


namespace se
{
	struct SerialTagOptional {};
	template<typename T> struct SerialTag<std::optional<T>> { typedef SerialTagOptional type; };
	template<> template<typename S, typename T>
	static bool Serial<SerialTagOptional>::serial(S& _serial, T _optional)
	{
		if constexpr (S::getWritingEnabled())
		{
			const bool exists = _optional.has_value();
			se_serial(_serial, exists, "e");
			if (exists)
			{
				se_serial(_serial, _optional.value(), "v");
			}
			return true;
		}
		else
		{
			bool exists = false;
			se_serial(_serial, exists, "e");
			if (exists)
			{
				_optional.emplace();
				se_serial(_serial, _optional.value(), "v");
			}
			else
			{
				_optional.reset();
			}
			return true;
		}
	}
}
