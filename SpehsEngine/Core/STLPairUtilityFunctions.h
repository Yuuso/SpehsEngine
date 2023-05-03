#pragma once

#include "SpehsEngine/Core/Serial/Serial.h"
#include "SpehsEngine/Core/ReadBuffer.h"
#include "SpehsEngine/Core/WriteBuffer.h"
#include <stdint.h>
#include <utility>


namespace se
{
	template<typename T1, typename T2>
	void writeToBuffer(WriteBuffer& writeBuffer, const std::pair<T1, T2>& pair)
	{
		se_write(writeBuffer, pair.first);
		se_write(writeBuffer, pair.second);
	}

	template<typename T1, typename T2>
	bool readFromBuffer(ReadBuffer& readBuffer, std::pair<T1, T2>& pair)
	{
		se_read(readBuffer, pair.first);
		se_read(readBuffer, pair.second);
		return true;
	}

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
