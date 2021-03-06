#pragma once

#include "SpehsEngine/Graphics/Primitive.h"


namespace se
{
	namespace graphics
	{
		enum class ShapeType
		{
			Unknown,

			Triangle = 3,
			Square,
			Pentagon,
			Hexagon,
			Heptagon,
			Octagon,
			Nonegon,
			Decagon,
			Circle,

			Cube,
			Sphere,
			// Capsule
			// Cylinder
			// Cone
		};

		class Shape : public Primitive
		{
		public:

			Shape(const unsigned int _numVertices);
			Shape(const ShapeType _type, const unsigned int _resolution = 0);
			~Shape();

			Shape(const Shape& _other) = delete;
			Shape& operator=(const Shape& _other) = delete;

			Shape(Shape&& _other) = delete;
			Shape& operator=(Shape&& _other) = delete;


			void generate(const unsigned int _numVertices);
			void generate(const ShapeType _type, const unsigned int _resolution = 0);

			void setPrimitiveType(const PrimitiveType _primitiveType) override;
			void setVertices(std::shared_ptr<VertexBuffer> _vertices) override;
			void setIndices(std::shared_ptr<IndexBuffer> _indices) override;

		private:

			void generateIndices();

			ShapeType type = ShapeType::Unknown;
		};
	}
}
