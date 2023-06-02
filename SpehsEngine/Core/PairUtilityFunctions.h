#pragma once

#include "SpehsEngine/Core/Serial/Serial.h"
#include <stdint.h>
#include <utility>


namespace se
{
	struct SerialTagPair {};
	template<typename T, typename U> struct SerialTag<std::pair<T, U>> { typedef SerialTagPair type; };
	template<> template<typename S, typename T>
	static bool se::Serial<SerialTagPair>::serial(S& _serial, T _pair)
	{
		se_serial(_serial, _pair.first, "f");
		se_serial(_serial, _pair.second, "s");
		return true;
	}
}
