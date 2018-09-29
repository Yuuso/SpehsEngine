#include "stdafx.h"
#include "SpehsEngine/Core/ColorUtilityFunctions.h"

#include "SpehsEngine/Core/StringOperations.h"
#include "SpehsEngine/Core/WriteBuffer.h"
#include "SpehsEngine/Core/ReadBuffer.h"
#include "boost/algorithm/clamp.hpp"

namespace se
{
	void writeToBuffer(WriteBuffer& writeBuffer, const Color& color)
	{
		writeBuffer.write(color.r);
		writeBuffer.write(color.g);
		writeBuffer.write(color.b);
		writeBuffer.write(color.a);
	}

	void readFromBuffer(ReadBuffer& readBuffer, Color& color)
	{
		readBuffer.read(color.r);
		readBuffer.read(color.g);
		readBuffer.read(color.b);
		readBuffer.read(color.a);
	}
	
	std::string toString(const Color& color)
	{
		return "[r:"
			+ std::to_string((unsigned char)(boost::algorithm::clamp(color.r, 0.0f, 1.0f) * 255.0f)) + ", g:"
			+ std::to_string((unsigned char)(boost::algorithm::clamp(color.g, 0.0f, 1.0f) * 255.0f)) + ", b:"
			+ std::to_string((unsigned char)(boost::algorithm::clamp(color.b, 0.0f, 1.0f) * 255.0f)) + ", a:"
			+ std::to_string((unsigned char)(boost::algorithm::clamp(color.a, 0.0f, 1.0f) * 255.0f)) + "]";
	}

	void brightness(Color& _color, const float _brightnessFactor)
	{
		_color.r *= _brightnessFactor;
		_color.g *= _brightnessFactor;
		_color.b *= _brightnessFactor;
	}
}
