#pragma once

#include "SpehsEngine/Core/Color.h"
#include "SpehsEngine/Core/Serial/Serial.h"


namespace se
{
	template<> template<typename S, typename T>
	static bool Serial<Color>::serial(S& _serial, T _color)
	{
		se_serial(_serial, _color.r, "r");
		se_serial(_serial, _color.g, "g");
		se_serial(_serial, _color.b, "b");
		se_serial(_serial, _color.a, "a");
		return true;
	}

	std::string toString(const Color& color);

	void brightnessAdd(Color& color, const float brightness, const bool clamp = false);
	void brightnessFactor(Color& color, const float brightness, const bool clamp = false);
	[[nodiscard]] Color brightnessAddNewColor(const Color& color, const float brightness, const bool clamp = false);
	[[nodiscard]] Color brightnessFactorNewColor(const Color& color, const float brightness, const bool clamp = false);

	Color mixColor(const Color& color1, const Color& color2, const float factor);

	// Generate colors

	Color oppositeColor(const Color& _color);

	/*
	hue: 0 - 360.0f
	saturation: 0 - 1.0f
	brightness: 0 - 1.0f
	*/
	Color colorHSB(const float _hue, const float _saturation, const float _brightness);
}
