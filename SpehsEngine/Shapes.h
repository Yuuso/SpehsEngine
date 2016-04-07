#pragma once

#include "Primitive.h"
#include "Vertex.h"

#include <vector>

//Make all shapes 1.0f x 1.0f by size

namespace spehs
{
	extern std::vector<Vertex> getShapeVertexArray(Shape shape);
	extern int getNumVertices(Shape shape);
}