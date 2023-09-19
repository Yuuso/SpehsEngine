#pragma once


namespace se::gfx
{
	class IndexBuffer;
	class VertexBuffer;

	class ShapeGenerator
	{
	public:

		ShapeGenerator() = default;
		~ShapeGenerator() = default;

		ShapeGenerator(const ShapeGenerator& _other) = delete;
		ShapeGenerator& operator=(const ShapeGenerator& _other) = delete;

		ShapeGenerator(ShapeGenerator&& _other) = delete;
		ShapeGenerator& operator=(ShapeGenerator&& _other) = delete;


		void clear();

		std::shared_ptr<VertexBuffer> getVertexBuffer(const ShapeType _shapeType, const ShapeParameters _shapeParams);
		std::shared_ptr<IndexBuffer> getIndexBuffer(const size_t _numVertices, const ShapeType _shapeType, const PrimitiveType _primitiveType);

	private:

		struct VertexBufferCache
		{
			VertexBufferCache() = delete;
			VertexBufferCache(const ShapeType _shapeType, const ShapeParameters _params, std::shared_ptr<VertexBuffer> _buffer)
				: type(_shapeType), params(_params), buffer(_buffer) {}
			~VertexBufferCache() = default;
			VertexBufferCache(const VertexBufferCache& _other) = delete;
			VertexBufferCache& operator=(const VertexBufferCache& _other) = delete;
			VertexBufferCache(VertexBufferCache&& _other) = delete;
			VertexBufferCache& operator=(VertexBufferCache&& _other) = delete;

			const ShapeType type;
			const ShapeParameters params;
			std::shared_ptr<VertexBuffer> buffer;
		};
		struct IndexBufferCache
		{
			IndexBufferCache() = delete;
			IndexBufferCache(const size_t _numVertices, const ShapeType _shapeType, const PrimitiveType _primitiveType, std::shared_ptr<IndexBuffer> _buffer)
				: numVertices(_numVertices), shapeType(_shapeType), primitiveType(_primitiveType), buffer(_buffer) {}
			~IndexBufferCache() = default;
			IndexBufferCache(const IndexBufferCache& _other) = delete;
			IndexBufferCache& operator=(const IndexBufferCache& _other) = delete;
			IndexBufferCache(IndexBufferCache&& _other) = delete;
			IndexBufferCache& operator=(IndexBufferCache&& _other) = delete;

			const size_t numVertices;
			const ShapeType shapeType;
			const PrimitiveType primitiveType;
			std::shared_ptr<IndexBuffer> buffer;
		};

		std::vector<std::unique_ptr<VertexBufferCache>> vertexBufferCaches;
		std::vector<std::unique_ptr<IndexBufferCache>> indexBufferCaches;
	};
}
