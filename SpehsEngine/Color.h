#pragma once
#include <algorithm>
#include <string>
#include "Vector.h"

namespace spehs
{
	class Color
	{
	public:
		class Component
		{
		public:
			Component();
			Component(const int i);
			Component(const unsigned char uc);

			operator unsigned char() const;

			bool operator>(const int i) const;
			bool operator<(const int i) const;
			bool operator>=(const int i) const;
			bool operator<=(const int i) const;
			
			void operator+=(const int i);
			void operator-=(const int i);
			Component operator+(const int i) const;
			Component operator-(const int i) const;

			void operator*=(const float f);
			void operator/=(const float f);
			Component operator*(const float f) const;
			Component operator/(const float f) const;

			
		private:
			unsigned char value;
		};


	public:
		Color();
		Color(const Component& _r, const Component& _g, const Component& _b);
		Color(const Component& _r, const Component& _g, const Component& _b, const Component& _a);
		Color(const unsigned char* rgbaByteBuffer);
		spehs::Color& operator=(const unsigned char* rgbaByteBuffer);
		Component& operator[](const int index);
		const Component& operator[](const int index) const;
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