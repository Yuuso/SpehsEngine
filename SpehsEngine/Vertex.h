
#pragma once

#include "Color.h"
#include "Vector.h"


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
		Vertex(const spehs::vec2 &_position);
		Vertex(const spehs::vec2 &_position, const UV &_uv);
		Vertex(const spehs::vec2 &_position, const Color &_color, const UV &_uv);

		spehs::vec2 position;
		Color color;
		UV uv;
	};

	extern inline spehs::vec3 toVec3(const Vertex& _vertex);
}