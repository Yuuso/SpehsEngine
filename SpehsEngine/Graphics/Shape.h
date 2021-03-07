#pragma once

#include "SpehsEngine/Graphics/Primitive.h"
#include "SpehsEngine/Graphics/ShapeGenerator.h"
#include "SpehsEngine/Graphics/Types.h"


namespace se
{
	namespace graphics
	{
		class Shape : public Primitive
		{
		public:

			Shape();
			~Shape();

			Shape(const Shape& _other) = delete;
			Shape& operator=(const Shape& _other) = delete;

			Shape(Shape&& _other) = delete;
			Shape& operator=(Shape&& _other) = delete;


			void generate(const unsigned int _numVertices, ShapeGenerator* _generator = nullptr);
			void generate(const ShapeType _type, const unsigned int _resolution = 0, ShapeGenerator* _generator = nullptr);

			void setPrimitiveType(const PrimitiveType _primitiveType) override;
			void setPrimitiveType(const PrimitiveType _primitiveType, ShapeGenerator& _generator);
			void setVertices(std::shared_ptr<VertexBuffer> _vertices) override;
			void setIndices(std::shared_ptr<IndexBuffer> _indices) override;

		private:

			void generateIndices(ShapeGenerator& _generator);

			ShapeType type = ShapeType::Unknown;
		};
	}
}
