#pragma once

#include "SpehsEngine/Graphics/Primitive.h"


namespace se::gfx
{
	class ShapeGenerator;

	class Shape : public Primitive
	{
	public:

		Shape() = default;
		~Shape() = default;

		Shape(const Shape& _other) = delete;
		Shape& operator=(const Shape& _other) = delete;

		Shape(Shape&& _other) = delete;
		Shape& operator=(Shape&& _other) = delete;


		void generate(const ShapeType _shapeType, const ShapeParameters _shapeParams = ShapeParameters(), ShapeGenerator* _generator = nullptr);

		void setPrimitiveType(const PrimitiveType _primitiveType) override;
		void setPrimitiveType(const PrimitiveType _primitiveType, ShapeGenerator& _generator);
		void setVertices(std::shared_ptr<VertexBuffer>) override;
		void setIndices(std::shared_ptr<IndexBuffer>) override;

		ShapeType getShapeType() const;

	private:

		void generateIndices(ShapeGenerator& _generator);

		ShapeType shapeType = ShapeType::Unknown;
	};
}
