#include "stdafx.h"

#include "SpehsEngine/Core/Vertex.h"



namespace se
{

	UV::UV() : u(0.0f), v(0.0f)
	{

	}
	UV::UV(const float &_u, const float &_v) : u(_u), v(_v)
	{

	}
	bool UV::operator==(const UV& _other) const
	{
		return (u == _other.u && v == _other.v);
	}
	void UV::setUV(const float &_u, const float &_v)
	{
		u = _u;
		v = _v;
	}

	Vertex::Vertex()
	{

	}
	Vertex::Vertex(const se::vec2 &_position) : position(_position)
	{

	}
	Vertex::Vertex(const se::vec2 &_position, const UV &_uv) : position(_position), uv(_uv)
	{

	}
	Vertex::Vertex(const se::vec2 &_position, const se::Color &_color, const UV &_uv) : position(_position), color(_color), uv(_uv)
	{

	}

	inline se::vec3 toVec3(const Vertex& _vertex)
	{
		return se::vec3(_vertex.position.x, _vertex.position.y, 0.0f);
	}
}