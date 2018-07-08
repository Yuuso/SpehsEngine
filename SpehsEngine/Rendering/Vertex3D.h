
#pragma once

#include "SpehsEngine/Core/Vector.h"
#include "SpehsEngine/Core/Color.h"


namespace spehs
{
	class Vertex3D
	{
	public:
		Vertex3D() {}
		Vertex3D(const spehs::vec3 _pos)
			: position(_pos), color(255, 255, 255, 255), uv(spehs::vec2::zero), normal(spehs::vec3::zero) {}
		Vertex3D(const spehs::vec3 _pos, spehs::vec2 _uv, spehs::vec3 _normal)
			: position(_pos), color(255, 255, 255, 255), uv(_uv), normal(_normal) {}
		Vertex3D(const spehs::vec3 _pos, spehs::Color _color, spehs::vec2 _uv, spehs::vec3 _normal)
			: position(_pos), color(_color), uv(_uv), normal(_normal) {}

		spehs::vec3 position;
		spehs::Color color;
		spehs::vec2 uv;
		spehs::vec3 normal;
	};
}