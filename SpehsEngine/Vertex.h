#pragma once

namespace SpehsEngine
{
	struct Position
	{
		float x;
		float y;
		float z;
		void setPosition(float _x, float _y)
		{
			x = _x;
			y = _y;
		}
		void setPosition(float _x, float _y, float _z)
		{
			x = _x;
			y = _y;
			z = _z;
		}
	};

	struct ColorRGBA8
	{
		ColorRGBA8() : r(0), g(0), b(0), a(255) {}
		ColorRGBA8(unsigned char R, unsigned char G, unsigned char B, unsigned char A = 255) : r(R), g(G), b(B), a(A) {}
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;
	};
	struct UV
	{
		float u;
		float v;
	};
	struct Vertex
	{
		Position position;
		ColorRGBA8 color;
		UV uv;

		void setPosition(float x, float y)
		{
			position.x = x;
			position.y = y;
		}
		void setPosition(float _x, float _y, float _z)
		{
			position.x = _x;
			position.y = _y;
			position.z = _z;
		}

		void setColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
		{
			color.r = r;
			color.g = g;
			color.b = b;
			color.a = a;
		}

		void setUV(float u, float v)
		{
			uv.u = u;
			uv.v = v;
		}
	};
}