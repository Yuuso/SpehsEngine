#include "Color.h"
#include "Exceptions.h"
#include "StringOperations.h"


namespace spehs
{
	Color::Component::Component() :
#if (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_FLOAT)
		value(1.0f)
#elif (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_BYTE)
		value(255)
#endif
	{
	}
	Color::Component::Component(const int i) :
#if (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_FLOAT)
		value(float(i) / 255.0f)
#elif (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_BYTE)
		value(i)
#endif
	{
	}
	Color::Component::Component(const char c) :
#if (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_FLOAT)
		value(float(c) / 255.0f)
#elif (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_BYTE)
		value(c)
#endif
	{
	}
	Color::Component::Component(const unsigned char uc) :
#if (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_FLOAT)
		value(float(uc) / 255.0f)
#elif (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_BYTE)
		value(uc)
#endif
	{
	}
	Color::Component::Component(const unsigned u) :
#if (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_FLOAT)
		value(float(u) / 255.0f)
#elif (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_BYTE)
		value(u)
#endif
	{
	}
	Color::Component::Component(const float f) :
#if (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_FLOAT)
		value(f)
#elif (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_BYTE)
		value(f * 255.0f)
#endif
	{
	}
	Color::Component::Component(const double d) :
#if (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_FLOAT)
		value(d)
#elif (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_BYTE)
		value(d * 255.0f)
#endif
	{
	}


	//Cast to...
	Color::Component::operator unsigned char() const
	{
#if (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_FLOAT)
		return value * 255.0f;
#elif (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_BYTE)
		return value;
#endif
	}

	Color::Component::operator float() const
	{
#if (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_FLOAT)
		return value;
#elif (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_BYTE)
		return float(value) / 255.0f;
#endif
	}


	//Operating with integers (native format)
	void Color::Component::operator+=(const int i)
	{
#if (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_FLOAT)
		value += float(i) / 255.0f;
#elif (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_BYTE)
		value += i;
#endif
	}

	void Color::Component::operator-=(const int i)
	{
#if (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_FLOAT)
		value -= float(i) / 255.0f;
#elif (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_BYTE)
		value -= i;
#endif
	}

	void Color::Component::operator*=(const int i)
	{
		exceptions::warning("Color::Component: integer multiplication called! This should never happen!");
	}

	void Color::Component::operator/=(const int i)
	{
		exceptions::warning("Color::Component: integer division called! This should never happen!");
	}

	Color::Component Color::Component::operator+(const int i) const
	{
#if (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_FLOAT)
		return value + float(i) /255.0f;
#elif (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_BYTE)
		return value + i;
#endif
	}

	Color::Component Color::Component::operator-(const int i) const
	{
#if (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_FLOAT)
		return value - float(i) / 255.0f;
#elif (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_BYTE)
		return value - i;
#endif
	}

	Color::Component Color::Component::operator*(const int i) const
	{
		exceptions::warning("Color::Component: integer multiplication called! This should never happen!");
		return value;
	}

	Color::Component Color::Component::operator/(const int i) const
	{
		exceptions::warning("Color::Component: integer division called! This should never happen!");
		return value;
	}

	bool Color::Component::operator>(const int i) const
	{
#if (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_FLOAT)
		return value > float(i) / 255.0f;
#elif (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_BYTE)
		return value > i;
#endif
	}

	bool Color::Component::operator<(const int i) const
	{
#if (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_FLOAT)
		return value < float(i) / 255.0f;
#elif (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_BYTE)
		return value < i;
#endif
	}

	bool Color::Component::operator>=(const int i) const
	{
#if (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_FLOAT)
		return value >= float(i) / 255.0f;
#elif (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_BYTE)
		return value >= i;
#endif
	}

	bool Color::Component::operator<=(const int i) const
	{
#if (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_FLOAT)
		return value <= float(i) / 255.0f;
#elif (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_BYTE)
		return value <= i;
#endif
	}


	//Operating with floats (needs converting)
	void Color::Component::operator+=(const float f)
	{
#if (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_FLOAT)
		value += f;
#elif (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_BYTE)
		value += int(f * 255.0f);
#endif
	}

	void Color::Component::operator-=(const float f)
	{
#if (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_FLOAT)
		value -= f;
#elif (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_BYTE)
		value -= int(f * 255.0f);
#endif
	}

	void Color::Component::operator*=(const float f)
	{
#if (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_FLOAT)
		value *= f;
#elif (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_BYTE)
		value = float(value) / 255.0f * f;
#endif
	}

	void Color::Component::operator/=(const float f)
	{
#if (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_FLOAT)
		value /= f;
#elif (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_BYTE)
		value = float(value) / 255.0f / f;
#endif
	}

	Color::Component Color::Component::operator+(const float f)
	{
#if (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_FLOAT)
		return value + f;
#elif (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_BYTE)
		return value + int(f * 255.0f);
#endif
	}

	Color::Component Color::Component::operator-(const float f)
	{
#if (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_FLOAT)
		return value - f;
#elif (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_BYTE)
		return value - int(f * 255.0f);
#endif
	}

	Color::Component Color::Component::operator*(const float f)
	{
#if (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_FLOAT)
		return value * f;
#elif (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_BYTE)
		return float(value) / 255.0f * f;
#endif
	}

	Color::Component Color::Component::operator/(const float f)
	{
#if (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_FLOAT)
		return value / f;
#elif (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_BYTE)
		return float(value) / 255.0f / f;
#endif
	}

	bool Color::Component::operator>(const float f)
	{
#if (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_FLOAT)
		return value > f;
#elif (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_BYTE)
		return value > int(f * 255.0f);
#endif
	}

	bool Color::Component::operator<(const float f)
	{
#if (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_FLOAT)
		return value < f;
#elif (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_BYTE)
		return value < int(f * 255.0f);
#endif
	}

	bool Color::Component::operator>=(const float f)
	{
#if (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_FLOAT)
		return value >= f;
#elif (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_BYTE)
		return value >= int(f * 255.0f);
#endif
	}

	bool Color::Component::operator<=(const float f)
	{
#if (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_FLOAT)
		return value <= f;
#elif (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_BYTE)
		return value <= int(f * 255.0f);
#endif
	}










	///////////
	// COLOR //

	Color::Color()
	{
	}

	Color::Color(const Component& _r, const Component& _g, const Component& _b) : r(_r), g(_g), b(_b),
#if (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_FLOAT)
		a(1.0f)
#elif (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_BYTE)
		a(255)
#endif
	{
	}

	Color::Color(const Component& _r, const Component& _g, const Component& _b, const Component& _a) : r(_r), g(_g), b(_b), a(_a)
	{
	}

	Color::Color(const Vector4<float>& vec) : 
#if (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_FLOAT)
		r(vec.r), g(vec.g), b(vec.b), a(vec.a)
#elif (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_BYTE)
		r(vec.r * 255.0f), g(vec.g * 255.0f), b(vec.b * 255.0f), a(vec.a * 255.0f)
#endif
	{
	}

	Color::Color(const unsigned char* rgbaByteBuffer)
	{
#if (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_FLOAT)
		//Must use individual component assignment
		r = rgbaByteBuffer[0];
		g = rgbaByteBuffer[1];
		b = rgbaByteBuffer[2];
		a = rgbaByteBuffer[3];
#elif (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_BYTE)
		//Optimization: can copy straight to the allocated memory as it is
		memcpy(&r, rgbaByteBuffer, sizeof(Component) * 4);
#endif
	}

	Color& Color::operator=(const unsigned char* rgbaByteBuffer)
	{
#if (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_FLOAT)
		//Must use individual component assignment
		r = rgbaByteBuffer[0];
		g = rgbaByteBuffer[1];
		b = rgbaByteBuffer[2];
		a = rgbaByteBuffer[3];
#elif (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_BYTE)
		//Optimization: can copy straight to the allocated memory as it is
		memcpy(&r, rgbaByteBuffer, sizeof(Component) * 4);
#endif
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
			(float)r * componentMultiplier,
			(float)g * componentMultiplier,
			(float)b * componentMultiplier,
			(float)a * componentMultiplier);
	}

	Color Color::operator/(const float componentDivider) const
	{
		return Color(
			(float)r * componentDivider,
			(float)g * componentDivider,
			(float)b * componentDivider,
			(float)a * componentDivider);
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
		r = std::max(0.0f, std::min(1.0f, _brightnessFactor * (float)r));
		g = std::max(0.0f, std::min(1.0f, _brightnessFactor * (float)g));
		b = std::max(0.0f, std::min(1.0f, _brightnessFactor * (float)b));
	}

	std::string Color::toString() const
	{
#if (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_FLOAT)
		return "[r:"
			+ spehs::toString((float)r, 2) + ", g:"
			+ spehs::toString((float)g, 2) + ", b:"
			+ spehs::toString((float)b, 2) + ", a:"
			+ spehs::toString((float)a, 2) + "]";
#elif (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_BYTE)
		return "[r:"
			+ std::to_string((unsigned char)r) + ", g:"
			+ std::to_string((unsigned char)g) + ", b:"
			+ std::to_string((unsigned char)b) + ", a:"
			+ std::to_string((unsigned char)a) + "]";
#endif
	}


	/*
	void operator+=(const int componentIncrementation)
	{
	r += componentIncrementation;
	g += componentIncrementation;
	b += componentIncrementation;
	a += componentIncrementation;
	}

	void operator-=(const int componentDecrementation)
	{
	r -= componentDecrementation;
	g -= componentDecrementation;
	b -= componentDecrementation;
	a -= componentDecrementation;
	}

	void operator+=(const float componentIncrementation)
	{
	r += componentIncrementation;
	g += componentIncrementation;
	b += componentIncrementation;
	a += componentIncrementation;
	}

	void operator-=(const float componentDecrementation)
	{
	r -= componentDecrementation;
	g -= componentDecrementation;
	b -= componentDecrementation;
	a -= componentDecrementation;
	}

	Color operator+(const int componentIncrementation) const
	{
	return Color(
	(unsigned char)r + componentIncrementation,
	(unsigned char)g + componentIncrementation,
	(unsigned char)b + componentIncrementation,
	(unsigned char)a + componentIncrementation);
	}

	Color operator-(const int componentDecrementation) const
	{
	return Color(
	(unsigned char)r - componentDecrementation,
	(unsigned char)g - componentDecrementation,
	(unsigned char)b - componentDecrementation,
	(unsigned char)a - componentDecrementation);
	}

	Color operator+(const float componentIncrementation) const
	{
	return Color(
	(float)r + componentIncrementation,
	(float)g + componentIncrementation,
	(float)b + componentIncrementation,
	(float)a + componentIncrementation);
	}

	Color operator-(const float componentDecrementation) const
	{
	return Color(
	(float)r - componentDecrementation,
	(float)g - componentDecrementation,
	(float)b - componentDecrementation,
	(float)a - componentDecrementation);
	}
	*/
}