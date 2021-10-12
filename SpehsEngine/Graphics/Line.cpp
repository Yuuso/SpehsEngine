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
			setName("line");
		}

		void Line::setPrimitiveType(const PrimitiveType _primitiveType)
		{
			se_assert_m(false, "Cannot change primitive type of Line primitive!");
		}
		void Line::setVertices(std::shared_ptr<VertexBuffer> _vertices)
		{
			se_assert_m(false, "Cannot set vertices for Line primitive!");
		}
		void Line::setIndices(std::shared_ptr<IndexBuffer> _vertices)
		{
			se_assert_m(false, "Cannot set indices for Line primitive!");
		}

		void Line::clear()
		{
			Primitive::setVertices(nullptr);
			Primitive::setIndices(nullptr);
		}
		void Line::addPoint(const glm::vec3& _point)
		{
			std::shared_ptr<VertexBuffer> vertexBuffer = getVertices();
			if (!vertexBuffer)
			{
				vertexBuffer = std::make_shared<VertexBuffer>();
				vertexBuffer->setAttributes(Position);
				Primitive::setVertices(vertexBuffer);
			}
			vertexBuffer->grow(1);
			const size_t newVertexIndex = vertexBuffer->size() - 1;
			vertexBuffer->get<Position>(newVertexIndex) = _point;

			if (newVertexIndex)
			{
				std::shared_ptr<IndexBuffer> indexBuffer = getIndices();
				if (!indexBuffer)
				{
					indexBuffer = std::make_shared<IndexBuffer>();
					Primitive::setIndices(indexBuffer);
				}
				indexBuffer->pushBack(static_cast<IndexType>(newVertexIndex - 1));
				indexBuffer->pushBack(static_cast<IndexType>(newVertexIndex));
			}
		}
		const glm::vec3& Line::getPoint(const size_t _index) const
		{
			std::shared_ptr<VertexBuffer> vertexBuffer = getVertices();
			se_assert(vertexBuffer && vertexBuffer->size() > _index);
			return std::as_const(*vertexBuffer.get()).get<Position>(_index);
		}
		glm::vec3& Line::getPoint(const size_t _index)
		{
			std::shared_ptr<VertexBuffer> vertexBuffer = getVertices();
			se_assert(vertexBuffer && vertexBuffer->size() > _index);
			return vertexBuffer->get<Position>(_index);
		}
		const size_t Line::numPoints() const
		{
			std::shared_ptr<VertexBuffer> vertexBuffer = getVertices();
			return vertexBuffer ? vertexBuffer->size() : 0;
		}
	}
}
