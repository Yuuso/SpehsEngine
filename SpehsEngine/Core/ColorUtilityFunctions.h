#pragma once
#include <string>
#include "SpehsEngine/Core/Color.h"

namespace se
{
	class WriteBuffer;
	class ReadBuffer;

	void writeToBuffer(WriteBuffer& writeBuffer, const Color& color);
	void readFromBuffer(ReadBuffer& readBuffer, Color& color);
	std::string toString(const Color& color);
	void brightness(Color& color, const float brightnessFactor);
}
