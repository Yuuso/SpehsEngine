
#include "Vertex.h"


namespace SpehsEngine
{
	Position::Position() : x(0.0f), y(0.0f), z(0.0f)
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

	ColorRGBA::ColorRGBA() : r(0), g(0), b(0), a(1)
	{

	}
	ColorRGBA::ColorRGBA(const unsigned char &_r, const unsigned char &_g, const unsigned char &_b, const unsigned char &_a) : r((float) _r / 255.0f), g((float) _g / 255.0f), b((float) _b / 255.0f), a((float) _a / 255.0f)
	{

	}
	ColorRGBA::ColorRGBA(const float &_r, const float &_g, const float &_b, const float &_a) : r(_r), g(_g), b(_b), a(_a)
	{

	}
	void ColorRGBA::setColor(const unsigned char &_r, const unsigned char &_g, const unsigned char &_b, const unsigned char &_a)
	{
		r = (float) _r / 255.0f;
		g = (float) _g / 255.0f;
		b = (float) _b / 255.0f;
		a = (float) _a / 255.0f;
	}
	void ColorRGBA::setColor(const float &_r, const float &_g, const float &_b, const float &_a)
	{
		r = _r;
		g = _g;
		b = _b;
		a = _a;
	}

	UV::UV() : u(0.0f), v(0.0f)
	{

	}
	void UV::setUV(const float &_u, const float &_v)
	{
		u = _u;
		v = _v;
	}

	Vertex::Vertex()
	{

	}
	Vertex::Vertex(const Position &_position, const ColorRGBA &_color, const UV &_uv) : position(_position), color(_color), uv(_uv) 
	{

	}
	void Vertex::setPosition(const float &_x, const float &_y)
	{
		position.setPosition(_x, _y);
	}
	void Vertex::setPosition(const float &_x, const float &_y, const float &_z)
	{
		position.setPosition(_x, _y, _z);
	}
	void Vertex::setColor(const unsigned char &_r, const unsigned char &_g, const unsigned char &_b, const unsigned char &_a)
	{
		color.setColor(_r, _g, _b, _a);
	}
	void Vertex::setColor(const float &_r, const float &_g, const float &_b, const float &_a)
	{
		color.setColor(_r, _g, _b, _a);
	}
	void Vertex::setUV(const float &_u, const float &_v)
	{
		uv.setUV(_u, _v);
	}
}