#pragma once
#include <glew.h>

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
		ColorRGBA8(GLubyte R, GLubyte G, GLubyte B, GLubyte A = 255) : r(R), g(G), b(B), a(A) {}
		GLubyte r;
		GLubyte g;
		GLubyte b;
		GLubyte a;
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

		void setColor(GLubyte r, GLubyte g, GLubyte b, GLubyte a)
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