#pragma once


namespace se
{
	// Linear Interpolation
	template <typename type>
	inline type lerp(type _a, type _b, float _value)
	{
		return static_cast<type>((1.0f - _value) * _a + _value * _b);
	}

	int factorial(int number);
}