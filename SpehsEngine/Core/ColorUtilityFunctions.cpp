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

	Color colorHSB(const float _hue, const float _saturation, const float _brightness)
	{
		se_assert(_hue >= 0.0f && _hue <= 360.0f &&
			_saturation >= 0.0f && _saturation <= 1.0f &&
			_brightness >= 0.0f && _brightness <= 1.0f);

		float chroma = 0.0f, smallest = 0.0f, intermediateValue = 0.0f;

		chroma = _brightness * _saturation;
		intermediateValue = chroma * (1.0f - fabs(fmod(_hue / 60.0f, 2.0f) - 1.0f));
		smallest = _brightness - chroma;

		if (_hue >= 0.0f && _hue < 60.0f)
			return Color(chroma + smallest, intermediateValue + smallest, smallest);
		else if (_hue >= 60.0f && _hue < 120.0f)
			return Color(intermediateValue + smallest, chroma + smallest, smallest);
		else if (_hue >= 120.0f && _hue < 180.0f)
			return Color(smallest, chroma + smallest, intermediateValue + smallest);
		else if (_hue >= 180.0f && _hue < 240.0f)
			return Color(smallest, intermediateValue + smallest, chroma + smallest);
		else if (_hue >= 240.0f && _hue < 300.0f)
			return Color(intermediateValue + smallest, smallest, chroma + smallest);
		else if (_hue >= 300.0f && _hue < 360.0f)
			return Color(chroma + smallest, smallest, intermediateValue + smallest);
		else
			return Color(smallest, smallest, smallest);
	}
	Color randomColor(rng::PRNG<unsigned>& _prng)
	{
		return Color(_prng.unit(), _prng.unit(), _prng.unit());
	}
	Color randomBrightColor(rng::PRNG<unsigned>& _prng)
	{
		return colorHSB(_prng.random<float>(0.0f, 360.0f), 1.0f, 0.5f);
	}
}
