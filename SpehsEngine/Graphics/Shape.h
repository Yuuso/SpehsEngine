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


			void generate(const unsigned _numVertices);

			void setPrimitiveType(const PrimitiveType _primitiveType) override;
			void setVertices(const std::vector<Vertex>& _vertices) override;
			void setIndices(const std::vector<uint16_t>& _indices) override;

		private:

			void generateIndices();
		};
	}
}
