#pragma once
#include <algorithm>
#include <string>
#include "Exceptions.h"
#include "Vector.h"

namespace spehs
{
	class Color
	{
	public:
		class Component
		{
		public:
			Component() : value(255) {}
			Component(const int i) : value(i) {}
			Component(const char c) : value(c) {}
			Component(const unsigned char uc) : value(uc) {}
			Component(const unsigned u) : value(u) {}
			Component(const float f) : value(float(f) * 255.0f) {}

			//Cast to...
			operator unsigned char() const { return value; }
			operator float() const { return float(value) / 255.f; }

			//Operating with integers (native format)
			void operator+=(const int i) { value += i; }
			void operator-=(const int i) { value -= i; }
			void operator*=(const int i) { exceptions::warning("Color::Component: integer multiplication called! This should never happen!"); }
			void operator/=(const int i) { exceptions::warning("Color::Component: integer division called! This should never happen!"); }
			Component operator+(const int i) const { return value + i; }
			Component operator-(const int i) const { return value - i; }
			Component operator*(const int i) const { exceptions::warning("Color::Component: integer multiplication called! This should never happen!"); return value; }
			Component operator/(const int i) const { exceptions::warning("Color::Component: integer division called! This should never happen!"); return value; }
			bool operator>(const int i) const { return value > i; }
			bool operator<(const int i) const { return value < i; }
			bool operator>=(const int i) const { return value >= i; }
			bool operator<=(const int i) const { return value <= i; }

			//Operating with floats (needs converting)
			void operator+=(const float f) { value += int(f * 255.0f); }
			void operator-=(const float f) { value -= int(f * 255.0f); }
			void operator*=(const float f) { value = float(value) / 255.0f * f; }
			void operator/=(const float f) { value = float(value) / 255.0f / f; }
			Component operator+(const float f) { return value + int(f * 255.0f); }
			Component operator-(const float f) { return value - int(f * 255.0f); }
			Component operator*(const float f) { return float(value) / 255.0f * f; }
			Component operator/(const float f) { return float(value) / 255.0f / f; }
			bool operator>(const float f){ return value > int(f * 255.0f); }
			bool operator<(const float f){ return value < int(f * 255.0f); }
			bool operator>=(const float f){ return value >= int(f * 255.0f); }
			bool operator<=(const float f){ return value <= int(f * 255.0f); }

		private:
			unsigned char value;
		};
	public:

		Color() {}
		Color(const Component& _r, const Component& _g, const Component& _b) : r(_r), g(_g), b(_b), a(255) {}
		Color(const Component& _r, const Component& _g, const Component& _b, const Component& _a) : r(_r), g(_g), b(_b), a(_a) {}
		Color(const Vector4<float>& vec) : r(vec.r * 255.0f), g(vec.g * 255.0f), b(vec.b * 255.0f), a(vec.a * 255.0f){}
		Color(const unsigned char* rgbaByteBuffer)
		{ 
			memcpy(&r, rgbaByteBuffer, sizeof(Component) * 4);
		}

		spehs::Color& operator=(const unsigned char* rgbaByteBuffer)
		{
			memcpy(&r, rgbaByteBuffer, sizeof(Component) * 4);
			return *this;
		}

		Component& operator[](const int index)
		{
			return *((Component*)this + index);
		}

		const Component& operator[](const int index) const
		{
			return *((Component*)this + index);
		}

		void operator*=(const float componentMultiplier)
		{
			r *= componentMultiplier;
			g *= componentMultiplier;
			b *= componentMultiplier;
			a *= componentMultiplier;
		}

		void operator/=(const float componentDivider)
		{
			r /= componentDivider;
			g /= componentDivider;
			b /= componentDivider;
			a /= componentDivider;
		}

		void operator+=(const Color& other)
		{
			r += (unsigned char)other.r;
			g += (unsigned char)other.g;
			b += (unsigned char)other.b;
			a += (unsigned char)other.a;
		}

		void operator-=(const Color& other)
		{
			r -= (unsigned char)other.r;
			g -= (unsigned char)other.g;
			b -= (unsigned char)other.b;
			a -= (unsigned char)other.a;
		}
		
		Color operator*(const float componentMultiplier) const
		{
			return Color(
				(float)r * componentMultiplier,
				(float)g * componentMultiplier,
				(float)b * componentMultiplier,
				(float)a * componentMultiplier);
		}

		Color operator/(const float componentDivider) const
		{
			return Color(
				(float)r * componentDivider,
				(float)g * componentDivider,
				(float)b * componentDivider,
				(float)a * componentDivider);
		}

		Color operator+(const Color& other) const
		{
			return Color(
				(unsigned char)r + (unsigned char)other.r,
				(unsigned char)g + (unsigned char)other.g,
				(unsigned char)b + (unsigned char)other.b,
				(unsigned char)a + (unsigned char)other.a);
		}

		Color operator-(const Color& other) const
		{
			return Color(
				(unsigned char)r - (unsigned char)other.r,
				(unsigned char)g - (unsigned char)other.g,
				(unsigned char)b - (unsigned char)other.b,
				(unsigned char)a - (unsigned char)other.a);
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

		bool operator!=(const Color& other) const
		{
			return
				(unsigned char)r != (unsigned char)other.r ||
				(unsigned char)g != (unsigned char)other.g ||
				(unsigned char)b != (unsigned char)other.b ||
				(unsigned char)a != (unsigned char)other.a;
		}

		bool operator==(const Color& other) const
		{
			return
				(unsigned char)r == (unsigned char)other.r &&
				(unsigned char)g == (unsigned char)other.g &&
				(unsigned char)b == (unsigned char)other.b &&
				(unsigned char)a == (unsigned char)other.a;
		}

		void brightness(const float _brightnessFactor)
		{
			r = std::max(0.0f, std::min(1.0f, _brightnessFactor * (float)r));
			g = std::max(0.0f, std::min(1.0f, _brightnessFactor * (float)g));
			b = std::max(0.0f, std::min(1.0f, _brightnessFactor * (float)b));
		}

		std::string toString() const
		{
			return "[r:"
				+ std::to_string((unsigned char)r) + ", g:"
				+ std::to_string((unsigned char)g) + ", b:"
				+ std::to_string((unsigned char)b) + ", a:"
				+ std::to_string((unsigned char)a) + "]";
		}

		Component r;
		Component g;
		Component b;
		Component a;
	};

	/*
	Abstract class to inherit from for classes that are 'colorable'.
	Meaning, that they intend to implement standardized setColor() and setAlpha() methods.
	Run time checks can be applied to any type to check if they inherit from Colorable, and thus would have an implementation for these methods.
	*/
	class Colorable
	{
	public:
		//Pure virtual methods
		virtual void setColor(const spehs::Color& _newColor) = 0;
		virtual void setAlpha(const spehs::Color::Component& _newAlpha) = 0;
	};
}