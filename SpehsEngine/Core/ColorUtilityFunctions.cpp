#include "stdafx.h"
#include "SpehsEngine/Core/ColorUtilityFunctions.h"

#include "glm/common.hpp"


namespace se
{
	std::string toString(const Color& color)
	{
		return "[r:"
			+ std::to_string((unsigned char)(glm::clamp(color.r, 0.0f, 1.0f) * 255.0f)) + ", g:"
			+ std::to_string((unsigned char)(glm::clamp(color.g, 0.0f, 1.0f) * 255.0f)) + ", b:"
			+ std::to_string((unsigned char)(glm::clamp(color.b, 0.0f, 1.0f) * 255.0f)) + ", a:"
			+ std::to_string((unsigned char)(glm::clamp(color.a, 0.0f, 1.0f) * 255.0f)) + "]";
	}

	void brightnessAdd(Color& _color, const float _brightness, const bool _clamp)
	{
		_color.r += _brightness;
		_color.g += _brightness;
		_color.b += _brightness;
		if (_clamp)
		{
			glm::clamp(_color.r, 0.0f, 1.0f);
			glm::clamp(_color.g, 0.0f, 1.0f);
			glm::clamp(_color.b, 0.0f, 1.0f);
		}
	}
	void brightnessFactor(Color& _color, const float _brightness, const bool _clamp)
	{
		_color.r *= _brightness;
		_color.g *= _brightness;
		_color.b *= _brightness;
		if (_clamp)
		{
			glm::clamp(_color.r, 0.0f, 1.0f);
			glm::clamp(_color.g, 0.0f, 1.0f);
			glm::clamp(_color.b, 0.0f, 1.0f);
		}
	}
	Color brightnessAddNewColor(const Color& _color, const float _brightness, const bool _clamp)
	{
		Color result = _color;
		brightnessAdd(result, _brightness, _clamp);
		return result;
	}
	Color brightnessFactorNewColor(const Color& _color, const float _brightness, const bool _clamp)
	{
		Color result = _color;
		brightnessFactor(result, _brightness, _clamp);
		return result;
	}

	Color mixColor(const Color& color1, const Color& color2, const float factor)
	{
		return Color(
			glm::mix(color1.r, color2.r, factor),
			glm::mix(color1.g, color2.g, factor),
			glm::mix(color1.b, color2.b, factor),
			glm::mix(color1.a, color2.a, factor)
		);
	}

	Color oppositeColor(const Color& _color)
	{
		return Color{ 1.0f - _color.r, 1.0f - _color.g, 1.0f - _color.b, _color.a };
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
}
