
#pragma once

#include "SpehsEngine/Core/Vector.h"
#include "SpehsEngine/Core/Color.h"


namespace se
{
	namespace rendering
	{
		class Vertex3D
		{
		public:
			Vertex3D() {}
			Vertex3D(const se::vec3 _pos)
				: position(_pos), color(255, 255, 255, 255), uv(se::vec2::zero), normal(se::vec3::zero) {}
			Vertex3D(const se::vec3 _pos, se::vec2 _uv, se::vec3 _normal)
				: position(_pos), color(255, 255, 255, 255), uv(_uv), normal(_normal) {}
			Vertex3D(const se::vec3 _pos, se::Color _color, se::vec2 _uv, se::vec3 _normal)
				: position(_pos), color(_color), uv(_uv), normal(_normal) {}

			se::vec3 position;
			se::Color color;
			se::vec2 uv;
			se::vec3 normal;
		};
	}
}