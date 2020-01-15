#include "stdafx.h"

#include "SpehsEngine/Core/Color.h"
#include "SpehsEngine/Core/StringOperations.h"

#include <boost/algorithm/clamp.hpp>

#include <cstring>



namespace se
{
	Color::Color(const float* rgbaBuffer)
	{
		memcpy(&r, rgbaBuffer, sizeof(float) * 4);
	}

	Color& Color::operator=(const float* rgbaBuffer)
	{
		memcpy(&r, rgbaBuffer, sizeof(float) * 4);
		return *this;
	}

	float& Color::operator[](const int index)
	{
		return *((float*)this + index);
	}

	const float& Color::operator[](const int index) const
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
}
