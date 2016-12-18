
#pragma once

#include "Geometry.h"
#include "Console.h"

#include <glm/vec2.hpp>



namespace spehs
{
	//Linear Interpolation
	template <typename type>
	inline type lerp(type _a, type _b, float _value)
	{
		return (1.0f - _value) * _a + _value * _b;
	}

	//Biliear Interpolation
	template <typename type>
	type bilerp(const glm::vec2& _x, const glm::vec2& _p1, const glm::vec2& _p2, const glm::vec2& _p3, const glm::vec2& _p4, const type _v1, const type _v2, const type _v3, const type _v4)
	{
		spehs::console::error("Don't use this stupid algorithm...");
		return;

		//glm::vec2 UV;

		//glm::vec2 e = _p2 - _p1;
		//glm::vec2 f = _p4 - _p1;
		//glm::vec2 g = _p1 - _p2 + _p3 - _p4;
		//glm::vec2 h = _x - _p1;

		//float k2 = spehs::cross2(g, f);
		//float k1 = spehs::cross2(e, f) + spehs::cross2(h, g);
		//float k0 = spehs::cross2(h, e);

		//float w = k1 * k1 - 4.0f * k0 * k2;
		//if (w < 0.0f) 
		//	UV = glm::vec2(-1.0f);
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

		//	UV = glm::vec2(u, v);
		//}

		//return _v1 + (_v2 - _v1) * UV.x + (_v4 - _v1) * UV.y + (_v1 - _v2 + _v3 - _v4) * UV.x *UV.y;
	}
}