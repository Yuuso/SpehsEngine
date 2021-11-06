#pragma once

namespace se
{
	class Color
	{
	public:
		constexpr Color();
		constexpr Color(const float _r, const float _g, const float _b);
		constexpr Color(const float _r, const float _g, const float _b, const float _a);
		explicit constexpr Color(uint32_t _rgbaValue);
		Color(const float* rgbaBuffer);

		Color& operator=(const float* rgbaBuffer);
		float& operator[](const int index);
		const float& operator[](const int index) const;
		void operator*=(const float componentMultiplier);
		void operator*=(const Color& other);
		void operator/=(const float componentDivider);
		void operator+=(const Color& other);
		void operator-=(const Color& other);
		Color operator*(const float componentMultiplier) const;
		Color operator*(const Color& other) const;
		Color operator/(const float componentDivider) const;
		Color operator+(const Color& other) const;
		Color operator-(const Color& other) const;

		bool operator!=(const Color& other) const;
		bool operator==(const Color& other) const;

		Color& withAlpha(float _alpha) { a = _alpha; return *this; }
		Color withAlpha(float _alpha) const { Color result = *this; result.a = _alpha; return result; }

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

	constexpr Color::Color(uint32_t _rgbaValue)
		: r(static_cast<float>((_rgbaValue & 0xff000000) >> 24) / 255.0f)
		, g(static_cast<float>((_rgbaValue & 0x00ff0000) >> 16) / 255.0f)
		, b(static_cast<float>((_rgbaValue & 0x0000ff00) >> 8)  / 255.0f)
		, a(static_cast<float>((_rgbaValue & 0x000000ff) >> 0)  / 255.0f)
	{}
}