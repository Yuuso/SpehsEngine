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

	constexpr HexColor toHexColor(const Color& _color)
	{
		constexpr uint32_t maxValue = 0x000000FF;
		constexpr float maxValuef = float(maxValue);
		const uint32_t r = uint32_t(_color.r * maxValuef) << 24;
		const uint32_t g = uint32_t(_color.g * maxValuef) << 16;
		const uint32_t b = uint32_t(_color.b * maxValuef) << 8;
		const uint32_t a = uint32_t(_color.a * maxValuef);
		return HexColor(r | g | b | a);
	}
}
