
#include "Vertex.h"


namespace spehs
{
	Position::Position() : x(0.0f), y(0.0f), z(0.0f)
	{

	}
	Position::Position(const float &_x, const float &_y, const float &_z) : x(_x), y(_y), z(_z)
	{

	}
	void Position::setPosition(const float &_x, const float &_y)
	{
		x = _x;
		y = _y;
	}
	void Position::setPosition(const float &_x, const float &_y, const float &_z)
	{
		x = _x;
		y = _y;
		z = _z;
	}
	bool Position::operator==(const Position& _other)
	{
		return (x == _other.x && y == _other.y && z == _other.z);
	}

	ColorRGBA::ColorRGBA() : rgba(1.0f)
	{

	}
	ColorRGBA::ColorRGBA(const unsigned char _r, const unsigned char _g, const unsigned char _b, const unsigned char _a) : rgba((float) _r / 255.0f, (float) _g / 255.0f, (float) _b / 255.0f, (float) _a / 255.0f)
	{

	}
	ColorRGBA::ColorRGBA(const float &_r, const float &_g, const float &_b, const float &_a) : rgba(_r, _g, _b, _a)
	{

	}
	void ColorRGBA::setColor(const unsigned char _r, const unsigned char _g, const unsigned char _b, const unsigned char _a)
	{
		rgba.r = (float) _r / 255.0f;
		rgba.g = (float) _g / 255.0f;
		rgba.b = (float) _b / 255.0f;
		rgba.a = (float) _a / 255.0f;
	}
	void ColorRGBA::setColor(const float &_r, const float &_g, const float &_b, const float &_a)
	{
		rgba.r = _r;
		rgba.g = _g;
		rgba.b = _b;
		rgba.a = _a;
	}
	void ColorRGBA::setColor(const glm::vec4 _color)
	{
		rgba = _color;
	}

	UV::UV() : u(0.0f), v(0.0f)
	{

	}
	UV::UV(const float &_u, const float &_v) : u(_u), v(_v)
	{

	}
	bool UV::operator==(const UV& _other)
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
	Vertex::Vertex(const Position &_position) : position(_position)
	{

	}
	Vertex::Vertex(const Position &_position, const ColorRGBA &_color, const UV &_uv) : position(_position), color(_color), uv(_uv) 
	{

	}
	
	Vertex3D::Vertex3D()
	{

	}
	Vertex3D::Vertex3D(const Position &_position) : position(_position)
	{

	}
	Vertex3D::Vertex3D(const Position &_position, const UV &_uv, const Position &_normal) : position(_position), uv(_uv), normal(_normal)
	{

	}
}