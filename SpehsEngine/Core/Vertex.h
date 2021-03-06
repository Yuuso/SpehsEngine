
#pragma once

#include "SpehsEngine/Core/Color.h"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace se
{
	class UV
	{
	public:
		UV();
		UV(const float &_u, const float &_v);

		bool operator==(const UV& _other) const;

		void setUV(const float &_u, const float &_v);

		float u;
		float v;
	};

	class Vertex
	{
	public:
		Vertex();
		Vertex(const glm::vec2 &_position);
		Vertex(const glm::vec2 &_position, const UV &_uv);
		Vertex(const glm::vec2 &_position, const Color &_color, const UV &_uv);

		glm::vec2 position;
		Color color;
		UV uv;
	};

	extern inline glm::vec3 toVec3(const Vertex& _vertex);
}