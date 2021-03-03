#include "stdafx.h"
#include "SpehsEngine/Graphics/Line.h"


namespace se
{
	namespace graphics
	{
		using namespace VertexAttribute;

		Line::Line()
			: Primitive()
		{
			Primitive::setPrimitiveType(PrimitiveType::Lines);
			clear();
		}

		void Line::setPrimitiveType(const PrimitiveType _primitiveType)
		{
			se_assert_m(false, "Cannot change primitive type of Line primitive!");
		}
		void Line::setVertices(const VertexBuffer& _vertices)
		{
			se_assert_m(false, "Cannot set vertices for Line primitive!");
		}
		void Line::setIndices(const IndexBuffer& _indices)
		{
			se_assert_m(false, "Cannot set indices for Line primitive!");
		}

		void Line::clear()
		{
			VertexBuffer emptyBuffer;
			emptyBuffer.setAttributes(Position
									  | Color0);
			Primitive::setVertices(emptyBuffer);
			Primitive::setIndices(IndexBuffer());
		}
		void Line::addPoint(const glm::vec3& _point, const Color& _color)
		{
			// Optimize!?
			VertexBuffer newVertices = getVertices();
			newVertices.grow(1);
			newVertices.get<Position>(newVertices.size() - 1) = _point;
			newVertices.get<Color0>(newVertices.size() - 1) = _color;
			Primitive::setVertices(newVertices);

			if (newVertices.size() > 1)
			{
				IndexBuffer newIndices = getIndices();
				newIndices.pushBack(static_cast<IndexType>(newVertices.size() - 2));
				newIndices.pushBack(static_cast<IndexType>(newVertices.size() - 1));
				Primitive::setIndices(newIndices);
			}
		}
	}
}
