#include <cstring>
#include "SpehsEngine/Core/Color.h"
#include "SpehsEngine/Core/Exceptions.h"
#include "SpehsEngine/Core/StringOperations.h"


namespace spehs
{
	Color::Component::Component()
		: value(255)
	{
	}

	Color::Component::Component(const int i)
		: value(std::min(255, std::max(0, i)))
	{
	}

	Color::Component::Component(const unsigned char uc)
		: value(uc)
	{
	}
	
	Color::Component::operator unsigned char() const
	{
		return value;
	}
	
	void Color::Component::operator+=(const int i)
	{		
		value = std::min(255, std::max(0, (int)value + i));
	}

	void Color::Component::operator-=(const int i)
	{
		value = std::min(255, std::max(0, (int)value - i));
	}
	
	Color::Component Color::Component::operator+(const int i) const
	{
		return (Component)std::min(255, std::max(0, (int)value + i));
	}

	Color::Component Color::Component::operator-(const int i) const
	{
		return (Component)std::min(255, std::max(0, (int)value - i));
	}

	bool Color::Component::operator>(const int i) const
	{
		return value > i;
	}

	bool Color::Component::operator<(const int i) const
	{
		return value < i;
	}

	bool Color::Component::operator>=(const int i) const
	{
		return value >= i;
	}

	bool Color::Component::operator<=(const int i) const
	{
		return value <= i;
	}

	void Color::Component::operator*=(const float f)
	{
		value = std::min(255, std::max(0, int((float)value * f)));
	}

	void Color::Component::operator/=(const float f)
	{
		value = std::min(255, std::max(0, int((float)value * f)));
	}

	Color::Component Color::Component::operator*(const float f) const
	{
		return (Component)std::min(255, std::max(0, int((float)value * f)));
	}

	Color::Component Color::Component::operator/(const float f) const
	{
		return (Component)std::min(255, std::max(0, int((float)value / f)));
	}










	///////////
	// COLOR //

	Color::Color()
	{
	}

	Color::Color(const Component& _r, const Component& _g, const Component& _b)
		: r(_r)
		, g(_g)
		, b(_b)
		, a(255)
	{
	}

	Color::Color(const Component& _r, const Component& _g, const Component& _b, const Component& _a)
		: r(_r)
		, g(_g)
		, b(_b)
		, a(_a)
	{
	}

	Color::Color(const unsigned char* rgbaByteBuffer)
	{
		memcpy(&r, rgbaByteBuffer, sizeof(Component) * 4);
	}

	Color& Color::operator=(const unsigned char* rgbaByteBuffer)
	{
		memcpy(&r, rgbaByteBuffer, sizeof(Component) * 4);
		return *this;
	}

	Color::Component& Color::operator[](const int index)
	{
		return *((Component*)this + index);
	}

	const Color::Component& Color::operator[](const int index) const
	{
		return *((Component*)this + index);
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
		r += (unsigned char)other.r;
		g += (unsigned char)other.g;
		b += (unsigned char)other.b;
		a += (unsigned char)other.a;
	}

	void Color::operator-=(const Color& other)
	{
		r -= (unsigned char)other.r;
		g -= (unsigned char)other.g;
		b -= (unsigned char)other.b;
		a -= (unsigned char)other.a;
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
			(unsigned char)r + (unsigned char)other.r,
			(unsigned char)g + (unsigned char)other.g,
			(unsigned char)b + (unsigned char)other.b,
			(unsigned char)a + (unsigned char)other.a);
	}

	Color Color::operator-(const Color& other) const
	{
		return Color(
			(unsigned char)r - (unsigned char)other.r,
			(unsigned char)g - (unsigned char)other.g,
			(unsigned char)b - (unsigned char)other.b,
			(unsigned char)a - (unsigned char)other.a);
	}

	bool Color::operator!=(const Color& other) const
	{
		return
			(unsigned char)r != (unsigned char)other.r ||
			(unsigned char)g != (unsigned char)other.g ||
			(unsigned char)b != (unsigned char)other.b ||
			(unsigned char)a != (unsigned char)other.a;
	}

	bool Color::operator==(const Color& other) const
	{
		return
			(unsigned char)r == (unsigned char)other.r &&
			(unsigned char)g == (unsigned char)other.g &&
			(unsigned char)b == (unsigned char)other.b &&
			(unsigned char)a == (unsigned char)other.a;
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
			+ std::to_string((unsigned char)r) + ", g:"
			+ std::to_string((unsigned char)g) + ", b:"
			+ std::to_string((unsigned char)b) + ", a:"
			+ std::to_string((unsigned char)a) + "]";
	}
}