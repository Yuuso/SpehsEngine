#pragma once
#include <algorithm>
#include <string>
#include "Vector.h"
#define SPEHS_COLOR_COMPONENT_TYPE_FLOAT 1
#define SPEHS_COLOR_COMPONENT_TYPE_BYTE 2
//Define the color component type to a value from above list
#define SPEHS_COLOR_COMPONENT_TYPE SPEHS_COLOR_COMPONENT_TYPE_FLOAT

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
			Component(const char c);
			Component(const unsigned char uc);
			Component(const unsigned u);
			Component(const float f);
			Component(const double d);

			//Cast to...
			operator unsigned char() const;
			operator float() const;

			//Operating with integers (native format)
			void operator+=(const int i);
			void operator-=(const int i);
			void operator*=(const int i);
			void operator/=(const int i);
			Component operator+(const int i) const;
			Component operator-(const int i) const;
			Component operator*(const int i) const;
			Component operator/(const int i) const;
			bool operator>(const int i) const;
			bool operator<(const int i) const;
			bool operator>=(const int i) const;
			bool operator<=(const int i) const;

			//Operating with floats (needs converting)
			void operator+=(const float f);
			void operator-=(const float f);
			void operator*=(const float f);
			void operator/=(const float f);
			Component operator+(const float f);
			Component operator-(const float f);
			Component operator*(const float f);
			Component operator/(const float f);
			bool operator>(const float f);
			bool operator<(const float f);
			bool operator>=(const float f);
			bool operator<=(const float f);

		private:
#if (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_FLOAT)
			float value;
#elif (SPEHS_COLOR_COMPONENT_TYPE == SPEHS_COLOR_COMPONENT_TYPE_BYTE)
			unsigned char value;
#endif
		};


	public:
		Color();
		Color(const Component& _r, const Component& _g, const Component& _b);
		Color(const Component& _r, const Component& _g, const Component& _b, const Component& _a);
		Color(const Vector4<float>& vec);
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

		/*
		void operator+=(const int componentIncrementation);
		void operator-=(const int componentDecrementation);
		void operator+=(const float componentIncrementation);
		void operator-=(const float componentDecrementation);
		Color operator+(const int componentIncrementation) const;
		Color operator-(const int componentDecrementation) const;
		Color operator+(const float componentIncrementation) const;
		Color operator-(const float componentDecrementation) const;
		*/

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