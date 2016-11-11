
#include "Vertex.h"
#include "Console.h"
#include "Primitive.h" //color



namespace spehs
{
	ColorRGBA::ColorRGBA() : rgba(1.0f)
	{

	}
	ColorRGBA::ColorRGBA(const glm::vec4 &_color) : rgba(_color)
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
	void ColorRGBA::setColor(const int& _color)
	{
		switch (_color)
		{
		case WHITE:
			setColor(1.0f, 1.0f, 1.0f, 1.0f);
			break;
		case BLACK:
			setColor(0, 0, 0, 1.0f);
			break;
		case RED:
			setColor(1.0f, 0, 0, 1.0f);
			break;
		case ORANGE:
			setColor(1.0f, 0.5f, 0, 1.0f);
			break;
		case YELLOW:
			setColor(1.0f, 1.0f, 0, 1.0f);
			break;
		case GREEN:
			setColor(0, 1.0f, 0, 1.0f);
			break;
		case CYAN:
			setColor(0, 1.0f, 1.0f, 1.0f);
			break;
		case BLUE:
			setColor(0, 0, 1.0f, 1.0f);
			break;
		case MAGENTA:
			setColor(1.0f, 0, 1.0f, 1.0f);
			break;
		default:
			console::error("Invalid CommonColor: " + std::to_string(_color));
			break;
		}
	}

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
	Vertex::Vertex(const glm::vec2 &_position) : position(_position)
	{

	}
	Vertex::Vertex(const glm::vec2 &_position, const UV &_uv) : position(_position), uv(_uv)
	{

	}
	Vertex::Vertex(const glm::vec2 &_position, const ColorRGBA &_color, const UV &_uv) : position(_position), color(_color), uv(_uv)
	{

	}

	inline glm::vec3 toVec3(const Vertex& _vertex)
	{
		return glm::vec3(_vertex.position.x, _vertex.position.y, 0.0f);
	}
}