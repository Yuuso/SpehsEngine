#include "stdafx.h"
#include "SpehsEngine/Graphics/Internal/Batch.h"

#include "SpehsEngine/Graphics/Internal/InternalUtilities.h"

#include "SpehsEngine/Core/SE_Assert.h"


namespace se
{
	namespace graphics
	{
		static constexpr uint32_t MAX_BUFFER_SIZE = 65536;

		Batch::Batch(const RenderInfo _renderInfo)
			: renderInfo(_renderInfo)
		{
			// TODO: Test BGFX_BUFFER_ALLOW_RESIZE?
			vertexBufferHandle = bgfx::createDynamicVertexBuffer(MAX_BUFFER_SIZE, Vertex::getVertexLayout());
			indexBufferHandle = bgfx::createDynamicIndexBuffer(MAX_BUFFER_SIZE);
			static_assert(sizeof(IndexType) == 2);
		}
		Batch::~Batch()
		{
			if (bgfx::isValid(vertexBufferHandle))
			{
				bgfx::destroy(vertexBufferHandle);
				vertexBufferHandle = BGFX_INVALID_HANDLE;
			}
			if (bgfx::isValid(indexBufferHandle))
			{
				bgfx::destroy(indexBufferHandle);
				indexBufferHandle = BGFX_INVALID_HANDLE;
			}
		}

		bool Batch::check(const RenderInfo _renderInfo) const
		{
			return renderInfo.renderFlags == _renderInfo.renderFlags &&
				   renderInfo.primitiveType == _renderInfo.primitiveType &&
				   renderInfo.shader == _renderInfo.shader;
		}
		bool Batch::check(const size_t numVertices, const size_t numIndices) const
		{
			if (numVertices > MAX_BUFFER_SIZE || numIndices > MAX_BUFFER_SIZE)
				log::error("Max batch size exceeded!");
			return (MAX_BUFFER_SIZE - vertices.size()) >= numVertices &&
				   (MAX_BUFFER_SIZE - indices.size()) >= numIndices;
		}

		[[nodiscard]] const BatchPosition& Batch::add(const std::vector<Vertex>& _vertices, const std::vector<IndexType>& _indices)
		{
			batchPositions.emplace_back(std::make_unique<BatchPosition>());
			batchPositions.back()->verticesStart = vertices.size();
			batchPositions.back()->verticesEnd = vertices.size() + _vertices.size();
			batchPositions.back()->indicesStart = indices.size();
			batchPositions.back()->indicesEnd = indices.size() + _indices.size();

			const size_t firstIndex = vertices.size();
			const size_t indicesOldSize = indices.size();

			indices.insert(indices.end(), _indices.begin(), _indices.end());
			if (firstIndex != 0)
			{
				for (size_t i = indicesOldSize; i < indices.size(); i++)
					indices[i] += (IndexType)firstIndex;
			}

			vertices.insert(vertices.end(), _vertices.begin(), _vertices.end());

			se_assert(vertices.size() <= MAX_BUFFER_SIZE && indices.size() <= MAX_BUFFER_SIZE);

			needsVertexBufferUpdate = true;
			needsIndexBufferUpdate = true;

			return *batchPositions.back().get();
		}
		void Batch::remove(const BatchPosition& _positionInBatch)
		{
			const size_t numRemovedIndices = _positionInBatch.indicesEnd - _positionInBatch.indicesStart;
			const size_t numRemovedVertices = _positionInBatch.verticesEnd - _positionInBatch.verticesStart;
			se_assert(numRemovedIndices > 0 && numRemovedVertices > 0);

			vertices.erase(vertices.begin() + _positionInBatch.verticesStart, vertices.begin() + _positionInBatch.verticesEnd);
			indices.erase(indices.begin() + _positionInBatch.indicesStart, indices.begin() + _positionInBatch.indicesEnd);

			for (size_t i = _positionInBatch.indicesStart; i < indices.size(); i++)
				indices[i] -= (IndexType)numRemovedVertices;

			auto itToRemove = batchPositions.end();
			for (auto it = batchPositions.begin(); it < batchPositions.end(); it++)
			{
				if (it->get() == &_positionInBatch)
				{
					se_assert(itToRemove == batchPositions.end());
					itToRemove = it;
					continue;
				}

				if (it->get()->verticesStart >= _positionInBatch.verticesStart)
				{
					it->get()->indicesStart -= numRemovedIndices;
					it->get()->indicesEnd -= numRemovedIndices;
					it->get()->verticesStart -= numRemovedVertices;
					it->get()->verticesEnd -= numRemovedVertices;
				}
			}
			se_assert(itToRemove != batchPositions.end());
			itToRemove->reset(batchPositions.back().release());
			batchPositions.pop_back();

			needsVertexBufferUpdate = true;
			needsIndexBufferUpdate = true;
		}

		void Batch::updateVertices(const BatchPosition& _positionInBatch, const std::vector<Vertex>& _vertices, const glm::mat4& _transformMatrix)
		{
			se_assert(_vertices.size() == (_positionInBatch.verticesEnd - _positionInBatch.verticesStart));
			const glm::mat4 normalMatrix = glm::mat4(glm::inverse(glm::transpose(glm::mat3(_transformMatrix))));
			glm::vec4 newVertex;
			for (size_t i = 0; i < _vertices.size(); i++)
			{
				// Vertices
				newVertex = _transformMatrix * glm::vec4(_vertices[i].position, 1.0f);
				vertices[_positionInBatch.verticesStart + i].position.x = newVertex.x;
				vertices[_positionInBatch.verticesStart + i].position.y = newVertex.y;
				vertices[_positionInBatch.verticesStart + i].position.z = newVertex.z;

				// Normals
				newVertex = normalMatrix * glm::vec4(_vertices[i].normal, 1.0f);
				newVertex = glm::normalize(newVertex);
				vertices[_positionInBatch.verticesStart + i].normal.x = newVertex.x;
				vertices[_positionInBatch.verticesStart + i].normal.y = newVertex.y;
				vertices[_positionInBatch.verticesStart + i].normal.z = newVertex.z;

				// UVs
				vertices[_positionInBatch.verticesStart + i].uv = _vertices[i].uv;

				// Color
				vertices[_positionInBatch.verticesStart + i].color = _vertices[i].color;
			}
			needsVertexBufferUpdate = true;
		}
		void Batch::updateIndices(const BatchPosition& _positionInBatch, const std::vector<IndexType>& _indices)
		{
			se_assert(_indices.size() == (_positionInBatch.indicesEnd - _positionInBatch.indicesStart));
			for (size_t i = 0; i < _indices.size(); i++)
				indices[i + _positionInBatch.indicesStart] = _indices[i];
			needsIndexBufferUpdate = true;
		}

		bool Batch::render(RenderContext& _renderContext)
		{
			if (vertices.size() == 0 && indices.size() == 0 && batchPositions.size() == 0)
				return false;
			se_assert(vertices.size() != 0 && indices.size() != 0 && batchPositions.size() != 0);

			updateBuffers();

			bgfx::setIndexBuffer(indexBufferHandle, 0, (uint32_t)indices.size());
			bgfx::setVertexBuffer(0, vertexBufferHandle);

			applyRenderState(renderInfo, _renderContext);
			bgfx::submit(_renderContext.currentViewId, renderInfo.shader->programHandle);
			return true;
		}

		void Batch::updateBuffers()
		{
			if (vertices.size() == 0)
				return;
			se_assert(indices.size() > 0);

			const bgfx::Memory* bufferMemory;
			// TODO: Can I do a partial update?
			if (needsIndexBufferUpdate)
			{
				bufferMemory = bgfx::copy(&indices[0], indices.size() * sizeof(indices[0]));
				bgfx::update(indexBufferHandle, 0, bufferMemory);
				needsIndexBufferUpdate = false;
			}
			if (needsVertexBufferUpdate)
			{
				bufferMemory = bgfx::copy(&vertices[0], vertices.size() * sizeof(vertices[0]));
				bgfx::update(vertexBufferHandle, 0, bufferMemory);
				needsVertexBufferUpdate = false;
			}
		}
	}
}
