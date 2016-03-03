#pragma once


namespace spehs
{
	class Position
	{
	public:
		Position();

		void setPosition(const float &_x, const float &_y);
		void setPosition(const float &_x, const float &_y, const float &_z);

		float x;
		float y;
		float z;
	};
	class ColorRGBA
	{
	public:
		ColorRGBA();
		ColorRGBA(const unsigned char &_r, const unsigned char &_g, const unsigned char &_b, const unsigned char &_a = 255);
		ColorRGBA(const float &_r, const float &_g, const float &_b, const float &_a = 1.0f);

		void setColor(const unsigned char &_r, const unsigned char &_g, const unsigned char &_b, const unsigned char &_a);
		void setColor(const float &_r, const float &_g, const float &_b, const float &_a);

		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;
	};
	class UV
	{
	public:
		UV();

		void setUV(const float &_u, const float &_v);

		float u;
		float v;
	};

	class Vertex
	{
	public:
		Vertex();
		Vertex(const Position &_position, const ColorRGBA &_color, const UV &_uv);

		void setPosition(const float &_x, const float &_y);
		void setPosition(const float &_x, const float &_y, const float &_z);
		void setColor(const unsigned char &_r, const unsigned char &_g, const unsigned char &_b, const unsigned char &_a);
		void setColor(const float &_r, const float &_g, const float &_b, const float &_a);
		void setUV(const float &_u, const float &_v);

		Position position;
		ColorRGBA color;
		UV uv;
	};
}