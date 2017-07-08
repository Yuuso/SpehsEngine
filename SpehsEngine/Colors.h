
#pragma once

#include "Exceptions.h"

#include <glm/vec4.hpp>

#include <string>
#include <algorithm>


namespace spehs
{
	class ColorRGBA
	{
	public:
		ColorRGBA();
		ColorRGBA(const glm::vec4 &_color);
		ColorRGBA(const unsigned char _r, const unsigned char _g, const unsigned char _b, const unsigned char _a = 255);
		ColorRGBA(const float &_r, const float &_g, const float &_b, const float &_a = 1.0f);

		void setColor(const unsigned char _r, const unsigned char _g, const unsigned char _b, const unsigned char _a);
		void setColor(const float &_r, const float &_g, const float &_b, const float &_a);
		void setColor(const glm::vec4 _color);
		void setColor(const int& _commonColor);

		glm::vec4 rgba;
	};

	struct Color32
	{
		Color32() : r(0), g(0), b(0), a(0){}
		Color32(const uint8_t _r, const uint8_t _g, const uint8_t _b, const uint8_t _a) : r(_r), g(_g), b(_b), a(_a){}
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;

		void brightness(const float _brightnessFactor)
		{
			r = std::min(255, int(_brightnessFactor * r));
			g = std::min(255, int(_brightnessFactor * g));
			b = std::min(255, int(_brightnessFactor * b));
		}
	};

	struct ColorF
	{
		ColorF() : r(0.0f), g(0.0f), b(0.0f), a(0.0f){}
		ColorF(const float _r, const float _g, const float _b, const float _a) : r(_r), g(_g), b(_b), a(_a){}
		ColorF operator*(const float _other)
		{
			return ColorF(r* _other, g* _other, b* _other, a* _other);
		}
		ColorF operator/(const float _other)
		{
			return ColorF(r / _other, g / _other, b / _other, a / _other);
		}
		void operator+=(const ColorF _other)
		{
			r += _other.r;
			g += _other.g;
			b += _other.b;
			a += _other.a;
		}

		Color32 toColor()
		{
			return Color32(r, g, b, a);
		}

		float r;
		float g;
		float b;
		float a;
	};

	enum CommonColor
	{
		WHITE = 0xFFFFFF,
		BLACK = 0x000000,
		RED = 0xFF0000,
		ORANGE = 0xFF8000,
		YELLOW = 0xFFFF00,
		GREEN = 0x00FF00,
		CYAN = 0x00FFFF,
		BLUE = 0x0000FF,
		MAGENTA = 0xFF00FF
	};

	static glm::vec4 commonColor(const CommonColor _color)
	{
		switch (_color)
		{
		case WHITE:
			return glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		case BLACK:
			return glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		case RED:
			return glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		case ORANGE:
			return glm::vec4(1.0f, 0.5f, 0.0f, 1.0f);
		case YELLOW:
			return glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
		case GREEN:
			return glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
		case CYAN:
			return glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
		case BLUE:
			return glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
		case MAGENTA:
			return glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
		default:
			exceptions::unexpectedError("Invalid CommonColor: " + std::to_string(_color));
			break;
		}
	}


	//TODO: a single color class to rule them all?
	/*

		unsigned char:
			+low memory consumption (4 bytes)
			-needs conversion to floating point for animating colors

		float:
			-high memory consumption (16 bytes)
			+easy to do all kinds of animation, since animation is easiest to handle with floating points
			+native format to pass on to shader?

	*/
	class Color
	{

	public:
		unsigned char& operator[](const int index)
		{
			return *((unsigned char*)this + index);
		}
		const unsigned char& operator[](const int index) const
		{
			return *((const unsigned char*)this + index);
		}

		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;
	};
}