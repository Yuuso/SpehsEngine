#include "stdafx.h"
#include "SpehsEngine/Graphics/Shape.h"


namespace se
{
	namespace graphics
	{
		Shape::Shape()
			: Primitive()
		{
		}
		Shape::~Shape()
		{
		}

		void Shape::generate(const unsigned int _numVertices, ShapeGenerator* _generator)
		{
			se_assert(_numVertices >= 3);
			const unsigned int numVertices = std::max(3u, _numVertices);

			ShapeType shapeType = (ShapeType)numVertices;
			unsigned int resolution = 0;
			if (numVertices >= (unsigned int)ShapeType::Circle)
			{
				shapeType = ShapeType::Circle;
				resolution = numVertices;
			}

			generate(shapeType, resolution, _generator);
		}
		void Shape::generate(const ShapeType _type, const unsigned int _resolution, ShapeGenerator* _generator)
		{
			type = _type;
			switch (type)
			{
				case se::graphics::ShapeType::Unknown:
				default:
					se_assert_m(false, "Unknown shape type!");
					return;

				case se::graphics::ShapeType::Triangle:
					setName("triangle");
					break;
				case se::graphics::ShapeType::Square:
					setName("square");
					break;
				case se::graphics::ShapeType::Pentagon:
					setName("pentagon");
					break;
				case se::graphics::ShapeType::Hexagon:
					setName("hexagon");
					break;
				case se::graphics::ShapeType::Heptagon:
					setName("heptagon");
					break;
				case se::graphics::ShapeType::Octagon:
					setName("octagon");
					break;
				case se::graphics::ShapeType::Nonegon:
					setName("nonegon");
					break;
				case se::graphics::ShapeType::Decagon:
					setName("decagon");
					break;
				case se::graphics::ShapeType::Circle:
					setName("circle");
					break;

				case se::graphics::ShapeType::Cube:
					setName("cube");
					enableRenderFlag(RenderFlag::CullBackFace);
					break;
				case se::graphics::ShapeType::Sphere:
					setName("sphere");
					enableRenderFlag(RenderFlag::CullBackFace);
					break;
			}

			if (_generator)
			{
				Primitive::setVertices(_generator->getVertexBuffer(_type, _resolution));
				generateIndices(*_generator);
			}
			else
			{
				ShapeGenerator localShapeGen;
				Primitive::setVertices(localShapeGen.getVertexBuffer(_type, _resolution));
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
		void Shape::setVertices(std::shared_ptr<VertexBuffer> _vertices)
		{
			se_assert_m(false, "Cannot set vertices for Shape primitive!");
		}
		void Shape::setIndices(std::shared_ptr<IndexBuffer>)
		{
			se_assert_m(false, "Cannot set indices for Shape primitive!");
		}

		void Shape::generateIndices(ShapeGenerator& _generator)
		{
			if (!getVertices() || getVertices()->size() == 0)
				return;

			Primitive::setIndices(_generator.getIndexBuffer(getVertices()->size(), type, getPrimitiveType()));
		}
	}
}
