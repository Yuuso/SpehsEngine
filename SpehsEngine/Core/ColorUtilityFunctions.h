#pragma once

#include "SpehsEngine/Core/Color.h"
#include "SpehsEngine/Core/RNG.h"

#include <string>

namespace se
{
	class WriteBuffer;
	class ReadBuffer;

	void writeToBuffer(WriteBuffer& writeBuffer, const Color& color);
	void readFromBuffer(ReadBuffer& readBuffer, Color& color);
	std::string toString(const Color& color);
	void brightness(Color& color, const float brightnessFactor);
	Color mixColor(const Color& color1, const Color& color2, const float factor);

	// Generate colors

	/*
	hue: 0 - 360.0f
	saturation: 0 - 1.0f
	brightness: 0 - 1.0f
	*/
	Color colorHSB(const float _hue, const float _saturation, const float _brightness);

	/*
	PRNG random called 3 times.
	*/
	Color randomColor(rng::PRNG<unsigned>& _prng);

	/*
	PRNG random called 1 time.
	*/
	Color randomBrightColor(rng::PRNG<unsigned>& _prng);
}
