#pragma once

namespace se
{
	class Color
	{
	public:
		constexpr Color();
		constexpr Color(const float _r, const float _g, const float _b);
		constexpr Color(const float _r, const float _g, const float _b, const float _a);
		Color(const float* rgbaBuffer);

		Color& operator=(const float* rgbaBuffer);
		float& operator[](const int index);
		const float& operator[](const int index) const;
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

		float r;
		float g;
		float b;
		float a;
	};


	constexpr Color::Color()
		: r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}

	constexpr Color::Color(const float _r, const float _g, const float _b)
		: r(_r), g(_g), b(_b), a(1.0f) {}

	constexpr Color::Color(const float _r, const float _g, const float _b, const float _a)
		: r(_r), g(_g), b(_b), a(_a) {}
}