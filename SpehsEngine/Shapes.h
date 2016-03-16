#pragma once

#include "Primitive.h"


//Make all shapes 1.0f x 1.0f by size

namespace spehs
{
	class Vertex;

	extern Vertex* getShapeVertexArray(Shape shape); //Deleting is handled by the caller of the function.	
	extern int getNumVertices(Shape shape);
}