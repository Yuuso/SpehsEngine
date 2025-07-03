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

	int factorial(int number);
}