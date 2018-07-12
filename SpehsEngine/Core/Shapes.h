#pragma once

#include "SpehsEngine/Core/Vertex.h"

#include <vector>

//Make all shapes 1.0f x 1.0f by size

namespace se
{
	enum Shape
	{
		BUTTON = 0,
		RIGHTANGLED_TRIANGLE = 1,
		ISOSCELES_TRIANGLE = 2
	};
	extern std::vector<Vertex> getShapeVertexArray(Shape shape);
	extern int getNumVertices(Shape shape);
}