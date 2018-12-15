#pragma once

#include "SpehsEngine/Core/Color.h"

#include <glm/vec3.hpp>


namespace se
{
	namespace rendering
	{
		enum
		{
			VERTEX_UPDATE_NONE			= 0,
			VERTEX_UPDATE_VERTEX		= (1 << 0),
			VERTEX_UPDATE_NORMAL		= (1 << 1),
			VERTEX_UPDATE_COLOR			= (1 << 2),
			VERTEX_UPDATE_ALL			= (VERTEX_UPDATE_VERTEX | VERTEX_UPDATE_NORMAL | VERTEX_UPDATE_COLOR)
		};

		class Vertex3D
		{
		public:
			Vertex3D() {}
			Vertex3D(const glm::vec3 _pos)
				: position(_pos)
				{}
			Vertex3D(const glm::vec3 _pos, glm::vec2 _uv, glm::vec3 _normal)
				: position(_pos)
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