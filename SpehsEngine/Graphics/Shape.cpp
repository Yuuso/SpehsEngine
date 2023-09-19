#include "stdafx.h"
#include "SpehsEngine/Graphics/Shape.h"

#include "SpehsEngine/Graphics/IndexBuffer.h"
#include "SpehsEngine/Graphics/VertexBuffer.h"
#include "SpehsEngine/Graphics/ShapeGenerator.h"


void Shape::generate(const ShapeType _shapeType, const ShapeParameters _shapeParams, ShapeGenerator* _generator)
{
	shapeType = _shapeType;
	switch (shapeType)
	{
		case ShapeType::Unknown:
		default:
			log::error("Unknown shape type!");
			return;

		case ShapeType::Triangle:
			setName("triangle");
			break;
		case ShapeType::Square:
			setName("square");
			break;
		case ShapeType::Pentagon:
			setName("pentagon");
			break;
		case ShapeType::Hexagon:
			setName("hexagon");
			break;
		case ShapeType::Heptagon:
			setName("heptagon");
			break;
		case ShapeType::Octagon:
			setName("octagon");
			break;
		case ShapeType::Nonegon:
			setName("nonegon");
			break;
		case ShapeType::Decagon:
			setName("decagon");
			break;
		case ShapeType::Circle:
			setName("circle");
			break;

		case ShapeType::Cube:
			setName("cube");
			enableRenderFlags(RenderFlag::CullBackFace);
			break;
		case ShapeType::Sphere:
			setName("sphere");
			enableRenderFlags(RenderFlag::CullBackFace);
			break;
	}

	if (_generator)
	{
		Primitive::setVertices(_generator->getVertexBuffer(shapeType, _shapeParams));
		generateIndices(*_generator);
	}
	else
	{
		ShapeGenerator localShapeGen;
		Primitive::setVertices(localShapeGen.getVertexBuffer(shapeType, _shapeParams));
		generateIndices(localShapeGen);
	}
}

void Shape::setPrimitiveType(const PrimitiveType _primitiveType)
{
	ShapeGenerator localShapeGen;
	setPrimitiveType(_primitiveType, localShapeGen);
}
void Shape::setPrimitiveType(const PrimitiveType _primitiveType, ShapeGenerator& _generator)
{
	Primitive::setPrimitiveType(_primitiveType);
	generateIndices(_generator);
}
void Shape::setVertices(std::shared_ptr<VertexBuffer>)
{
	se_assert_m(false, "Cannot directly set vertices for Shape primitive!");
}
void Shape::setIndices(std::shared_ptr<IndexBuffer>)
{
	se_assert_m(false, "Cannot directly set indices for Shape primitive!");
}

void Shape::generateIndices(ShapeGenerator& _generator)
{
	if (!getVertices() || getVertices()->size() == 0)
		return;

	Primitive::setIndices(_generator.getIndexBuffer(getVertices()->size(), shapeType, getPrimitiveType()));
}

ShapeType Shape::getShapeType() const
{
	return shapeType;
}
