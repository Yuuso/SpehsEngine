#pragma once

#include "SpehsEngine/Graphics/Primitive.h"


namespace se::gfx
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
		void setVertices(std::shared_ptr<VertexBuffer> _vertices) override;
		void setIndices(std::shared_ptr<IndexBuffer> _indices) override;

		void clear();
		void addPoint(const glm::vec3& _point);
		const glm::vec3& getPoint(const size_t _index) const;
		glm::vec3& getPoint(const size_t _index);
		const size_t numPoints() const;
	};
}
