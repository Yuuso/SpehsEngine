#pragma once

#include "SpehsEngine/Core/Color.h"

#include <glm/vec3.hpp>


namespace se
{
	namespace rendering
	{
		class Vertex3D
		{
		public:
			Vertex3D() {}
			Vertex3D(const glm::vec3 _pos)
				: position(_pos)
				, color(255, 255, 255, 255) {}
			Vertex3D(const glm::vec3 _pos, glm::vec2 _uv, glm::vec3 _normal)
				: position(_pos), color(255, 255, 255, 255)
				, uv(_uv)
				, normal(_normal) {}
			Vertex3D(const glm::vec3 _pos, se::Color _color, glm::vec2 _uv, glm::vec3 _normal)
				: position(_pos)
				, color(_color)
				, uv(_uv)
				, normal(_normal) {}

			glm::vec3 position;
			se::Color color;
			glm::vec2 uv;
			glm::vec3 normal;
		};
	}
}