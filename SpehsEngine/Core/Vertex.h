
#pragma once

#include "SpehsEngine/Core/Vector.h"
#include "SpehsEngine/Core/Color.h"


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
		Vertex(const se::vec2 &_position);
		Vertex(const se::vec2 &_position, const UV &_uv);
		Vertex(const se::vec2 &_position, const Color &_color, const UV &_uv);

		se::vec2 position;
		Color color;
		UV uv;
	};

	extern inline se::vec3 toVec3(const Vertex& _vertex);
}