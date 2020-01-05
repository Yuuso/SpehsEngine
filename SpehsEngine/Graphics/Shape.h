#pragma once

#include "SpehsEngine/Graphics/Primitive.h"


namespace se
{
	namespace graphics
	{
		class Shape : public Primitive
		{
		public:

			Shape(const unsigned _numVertices = 4);
			~Shape();

			Shape(const Shape& _other) = delete;
			Shape& operator=(const Shape& _other) = delete;

			Shape(Shape&& _other) = delete;
			Shape& operator=(Shape&& _other) = delete;


			void recreate(const unsigned _numVertices);
		};
	}
}
