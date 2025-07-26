#pragma once


namespace se
{
	constexpr double lerp(const double _a, const double _b, const double _amount)
	{
		return (1.0 - _amount) * _a + _amount * _b;
	}
	constexpr float lerp(const float _a, const float _b, const float _amount)
	{
		return (1.0f - _amount) * _a + _amount * _b;
	}
	constexpr Color lerp(const Color& _a, const Color& _b, const float _amount)
	{
		return Color(
			lerp(_a.r, _b.r, _amount),
			lerp(_a.g, _b.g, _amount),
			lerp(_a.b, _b.b, _amount),
			lerp(_a.a, _b.a, _amount));
	}

	int factorial(int number);
}