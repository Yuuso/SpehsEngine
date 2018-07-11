
#include "SpehsEngine/Core/Color.h"
#include "SpehsEngine/Core/Exceptions.h"
#include "SpehsEngine/Core/StringOperations.h"

#include <boost/algorithm/clamp.hpp>

#include <cstring>



namespace spehs
{
	Color::Color()
		: r(1.0f)
		, g(1.0f)
		, b(1.0f)
		, a(1.0f)
	{
	}

	Color::Color(const float _r, const float _g, const float _b)
		: r(_r)
		, g(_g)
		, b(_b)
		, a(1.0f)
	{
	}

	Color::Color(const float _r, const float _g, const float _b, const float _a)
		: r(_r)
		, g(_g)
		, b(_b)
		, a(_a)
	{
	}

	Color::Color(const float* rgbaBuffer)
	{
		memcpy(&r, rgbaBuffer, sizeof(float) * 4);
	}

	Color& Color::operator=(const float* rgbaBuffer)
	{
		memcpy(&r, rgbaBuffer, sizeof(float) * 4);
		return *this;
	}

	float Color::operator[](const int index)
	{
		return *((float*)this + index);
	}

	const float Color::operator[](const int index) const
	{
		return *((float*)this + index);
	}

	void Color::operator*=(const float componentMultiplier)
	{
		r *= componentMultiplier;
		g *= componentMultiplier;
		b *= componentMultiplier;
		a *= componentMultiplier;
	}

	void Color::operator/=(const float componentDivider)
	{
		r /= componentDivider;
		g /= componentDivider;
		b /= componentDivider;
		a /= componentDivider;
	}

	void Color::operator+=(const Color& other)
	{
		r += other.r;
		g += other.g;
		b += other.b;
		a += other.a;
	}

	void Color::operator-=(const Color& other)
	{
		r -= other.r;
		g -= other.g;
		b -= other.b;
		a -= other.a;
	}

	Color Color::operator*(const float componentMultiplier) const
	{
		return Color(
			r * componentMultiplier,
			g * componentMultiplier,
			b * componentMultiplier,
			a * componentMultiplier);
	}

	Color Color::operator/(const float componentDivider) const
	{
		return Color(
			r * componentDivider,
			g * componentDivider,
			b * componentDivider,
			a * componentDivider);
	}

	Color Color::operator+(const Color& other) const
	{
		return Color(
			r + other.r,
			g + other.g,
			b + other.b,
			a + other.a);
	}

	Color Color::operator-(const Color& other) const
	{
		return Color(
			r - other.r,
			g - other.g,
			b - other.b,
			a - other.a);
	}

	bool Color::operator!=(const Color& other) const
	{
		return
			r != other.r ||
			g != other.g ||
			b != other.b ||
			a != other.a;
	}

	bool Color::operator==(const Color& other) const
	{
		return
			r == other.r &&
			g == other.g &&
			b == other.b &&
			a == other.a;
	}

	void Color::brightness(const float _brightnessFactor)
	{
		r *= _brightnessFactor;
		g *= _brightnessFactor;
		b *= _brightnessFactor;
	}

	std::string Color::toString() const
	{
		return "[r:"
			+ std::to_string((unsigned char)(boost::algorithm::clamp(r, 0.0f, 1.0f) * 255.0f)) + ", g:"
			+ std::to_string((unsigned char)(boost::algorithm::clamp(g, 0.0f, 1.0f) * 255.0f)) + ", b:"
			+ std::to_string((unsigned char)(boost::algorithm::clamp(b, 0.0f, 1.0f) * 255.0f)) + ", a:"
			+ std::to_string((unsigned char)(boost::algorithm::clamp(a, 0.0f, 1.0f) * 255.0f)) + "]";
	}
}