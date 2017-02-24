
#pragma once

#include "Colors.h"

#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/fwd.hpp>


namespace spehs
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
		Vertex(const glm::vec2 &_position, const ColorRGBA &_color, const UV &_uv);

		glm::vec2 position;
		ColorRGBA color;
		UV uv;
	};

	extern inline glm::vec3 toVec3(const Vertex& _vertex);
}