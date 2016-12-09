
#pragma once


namespace spehs
{
	//Linear interpolation
	template <typename type>
	inline type lerp(type _a, type _b, float _value)
	{
		return (1.0f - _value) * _a + _value * _b;
	}
}