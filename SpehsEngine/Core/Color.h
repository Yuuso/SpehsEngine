
#pragma once

#include "SpehsEngine/Core/Vector.h"

#include <algorithm>
#include <string>


namespace se
{
	class Color
	{
	public:

	public:
		Color();
		Color(const float _r, const float _g, const float _b);
		Color(const float _r, const float _g, const float _b, const float _a);
		Color(const float* rgbaBuffer);
		se::Color& operator=(const float* rgbaBuffer);
		float operator[](const int index);
		const float operator[](const int index) const;
		void operator*=(const float componentMultiplier);
		void operator/=(const float componentDivider);
		void operator+=(const Color& other);
		void operator-=(const Color& other);
		Color operator*(const float componentMultiplier) const;
		Color operator/(const float componentDivider) const;
		Color operator+(const Color& other) const;
		Color operator-(const Color& other) const;

		bool operator!=(const Color& other) const;
		bool operator==(const Color& other) const;
		void brightness(const float _brightnessFactor);
		std::string toString() const;

		float r;
		float g;
		float b;
		float a;
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
		virtual void setColor(const se::Color& _newColor) = 0;
		virtual void setAlpha(const float _newAlpha) = 0;
	};
}