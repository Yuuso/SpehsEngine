#include "stdafx.h"
#include "SpehsEngine/Graphics/Internal/Batch.h"

#include "SpehsEngine/Core/SE_Assert.h"


namespace se
{
	namespace graphics
	{
		Batch::Batch(const RenderFlagsType _renderFlags, const Shader& _shader)
			: renderFlags(_renderFlags)
			, shader(_shader)
		{
			vertexBufferHandle = bgfx::createDynamicVertexBuffer(1024, Vertex::getVertexLayout());
			indexBufferHandle = bgfx::createDynamicIndexBuffer(1024);
			static_assert(sizeof(IndexType) == 2);
		}
		Batch::~Batch()
		{
			bgfx::destroy(vertexBufferHandle);
			bgfx::destroy(indexBufferHandle);
		}

		bool Batch::check(const RenderFlagsType _renderFlags) const
		{
			return renderFlags == _renderFlags;
		}
		bool Batch::check(const size_t numVertices, const size_t numIndices) const
		{
			return (1024 - vertices.size()) > numVertices &&
				   (1024 - indices.size()) > numIndices;
		}

		[[nodiscard]] BatchPosition Batch::add(const std::vector<Vertex>& _vertices, const std::vector<IndexType>& _indices)
		{
			BatchPosition result;
			result.verticesStart = vertices.size();
			result.verticesEnd = result.verticesStart + _vertices.size();
			result.indicesStart = indices.size();
			result.indicesEnd = result.indicesStart + _indices.size();

			const size_t firstIndex = vertices.size();
			const size_t indicesOldSize = indices.size();

			indices.insert(indices.end(), _indices.begin(), _indices.end());
			if (firstIndex != 0)
			{
				for (size_t i = indicesOldSize; i < indices.size(); i++)
					indices[i] += (IndexType)firstIndex;
			}

			vertices.insert(vertices.end(), _vertices.begin(), _vertices.end());

			se_assert(vertices.size() < 1024 && indices.size() < 1024);
			updateBuffers();

			return result;
		}
		void Batch::remove(const BatchPosition& _positionInBatch)
		{
			vertices.erase(vertices.begin() + _positionInBatch.verticesStart, vertices.begin() + _positionInBatch.verticesEnd);
			indices.erase(indices.begin() + _positionInBatch.indicesStart, indices.begin() + _positionInBatch.indicesEnd);

			const size_t numRemovedIndices = _positionInBatch.indicesEnd - _positionInBatch.indicesStart;
			for (size_t i = _positionInBatch.indicesStart; i < indices.size(); i++)
				indices[i] -= (IndexType)numRemovedIndices;

			updateBuffers();
		}

		void Batch::render(RenderContext& _renderContext) const
		{
			//bgfx::TransientIndexBuffer tib;
			//bgfx::TransientVertexBuffer tvb;
			//bgfx::allocTransientIndexBuffer(&tib, indices.size());
			//bgfx::allocTransientVertexBuffer(&tvb, vertices.size(), Vertex::getVertexLayout());
			//memcpy(tib.data, &indices[0], indices.size() * sizeof(indices[0]));
			//memcpy(tvb.data, &vertices[0], vertices.size() * sizeof(vertices[0]));

			//bgfx::setIndexBuffer(&tib, 0, indices.size());
			//bgfx::setVertexBuffer(0, &tvb, 0, vertices.size());

			//bgfx::setState(0
			//			   | BGFX_STATE_BLEND_ALPHA
			//			   | BGFX_STATE_WRITE_RGB
			//			   | BGFX_STATE_WRITE_A
			//			   | BGFX_STATE_WRITE_Z
			//			   | BGFX_STATE_DEPTH_TEST_LESS
			//			   | BGFX_STATE_CULL_CCW
			//			   | BGFX_STATE_MSAA);

			//bgfx::submit(0, programHandle);

			bgfx::setIndexBuffer(indexBufferHandle);
			bgfx::setVertexBuffer(0, vertexBufferHandle);

			bgfx::setState(0
						   | BGFX_STATE_BLEND_ALPHA
						   | BGFX_STATE_WRITE_RGB
						   | BGFX_STATE_WRITE_A
						   | BGFX_STATE_WRITE_Z
						   | BGFX_STATE_DEPTH_TEST_LESS
						   | BGFX_STATE_CULL_CCW
						   | BGFX_STATE_MSAA);

			bgfx::submit(_renderContext.currentViewId, shader.programHandle);
		}

		void Batch::updateBuffers()
		{
			if (vertices.size() == 0)
				return;
			se_assert(indices.size() > 0);
			const bgfx::Memory* bufferMemory;
			bufferMemory = bgfx::copy(&indices[0], indices.size() * sizeof(indices[0]));
			bgfx::update(indexBufferHandle, 0, bufferMemory);
			bufferMemory = bgfx::copy(&vertices[0], vertices.size() * sizeof(vertices[0]));
			bgfx::update(vertexBufferHandle, 0, bufferMemory);
		}
	}
}
