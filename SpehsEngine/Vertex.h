
#pragma once

#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/fwd.hpp>


namespace spehs
{
	class Position
	{
	public:
		Position();
		Position(const float &_x, const float &_y);

		void setPosition(const float &_x, const float &_y);

		bool operator==(const Position &_other) const;

		float x;
		float y;
	};
	class ColorRGBA
	{
	public:
		ColorRGBA();
		ColorRGBA(const glm::vec4 &_color);
		ColorRGBA(const unsigned char _r, const unsigned char _g, const unsigned char _b, const unsigned char _a = 255);
		ColorRGBA(const float &_r, const float &_g, const float &_b, const float &_a = 1.0f);

		void setColor(const unsigned char _r, const unsigned char _g, const unsigned char _b, const unsigned char _a);
		void setColor(const float &_r, const float &_g, const float &_b, const float &_a);
		void setColor(const glm::vec4 _color);
		void setColor(const int& _commonColor);

		glm::lowp_f32vec4 rgba;
	};
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
		Vertex(const Position &_position);
		Vertex(const Position &_position, const UV &_uv);
		Vertex(const Position &_position, const ColorRGBA &_color, const UV &_uv);

		Position position;
		ColorRGBA color;
		UV uv;
	};

	extern inline glm::vec3 toVec3(const Vertex& _vertex);
	extern inline glm::vec3 toVec3(const Position& _position);
}