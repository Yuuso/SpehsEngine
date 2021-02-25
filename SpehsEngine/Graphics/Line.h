#pragma once

#include "SpehsEngine/Graphics/Primitive.h"


namespace se
{
	namespace graphics
	{
		class Line : public Primitive
		{
		public:

			Line();
			~Line() = default;

			Line(const Line& _other) = delete;
			Line& operator=(const Line& _other) = delete;

			Line(Line&& _other) = delete;
			Line& operator=(Line&& _other) = delete;


			void setPrimitiveType(const PrimitiveType _primitiveType) override;
			void setVertices(const VertexBuffer& _vertices) override;
			void setIndices(const std::vector<IndexType>& _indices) override;

			void clear();
			void addPoint(const glm::vec3& _point, const Color& _color = Color());
		};
	}
}
