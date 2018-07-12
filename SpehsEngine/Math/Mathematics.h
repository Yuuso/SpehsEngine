#pragma once
#include "SpehsEngine/Core/Vector.h"



namespace se
{
	//Linear Interpolation
	template <typename type>
	inline type lerp(type _a, type _b, float _value)
	{
		return (1.0f - _value) * _a + _value * _b;
	}

	//Biliear Interpolation
	template <typename type>
	type bilerp(const se::vec2& _x, const se::vec2& _p1, const se::vec2& _p2, const se::vec2& _p3, const se::vec2& _p4, const type _v1, const type _v2, const type _v3, const type _v4)
	{
		//se::console::error("Don't use this stupid algorithm...");
		return;

		//se::vec2 UV;

		//se::vec2 e = _p2 - _p1;
		//se::vec2 f = _p4 - _p1;
		//se::vec2 g = _p1 - _p2 + _p3 - _p4;
		//se::vec2 h = _x - _p1;

		//float k2 = se::cross2(g, f);
		//float k1 = se::cross2(e, f) + se::cross2(h, g);
		//float k0 = se::cross2(h, e);

		//float w = k1 * k1 - 4.0f * k0 * k2;
		//if (w < 0.0f) 
		//	UV = se::vec2(-1.0f);
		//else
		//{
		//	w = sqrt(w);

		//	float v1 = (-k1 - w) / (2.0f * k2);
		//	float u1 = (h.x - f.x * v1) / (e.x + g.x * v1);

		//	float v2 = (-k1 + w) / (2.0f * k2);
		//	float u2 = (h.x - f.x * v2) / (e.x + g.x * v2);

		//	float u = u1;
		//	float v = v1;

		//	if (v<0.0 || v>1.0 || u<0.0 || u>1.0)
		//	{
		//		u = u2;
		//		v = v2;
		//	}
		//	if (v<0.0 || v>1.0 || u<0.0 || u>1.0)
		//	{
		//		u = -1.0;
		//		v = -1.0;
		//	}

		//	UV = se::vec2(u, v);
		//}

		//return _v1 + (_v2 - _v1) * UV.x + (_v4 - _v1) * UV.y + (_v1 - _v2 + _v3 - _v4) * UV.x *UV.y;
	}

	int factorial(int number);
}