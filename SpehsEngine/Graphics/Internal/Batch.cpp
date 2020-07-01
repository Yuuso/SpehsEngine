#include "stdafx.h"
#include "SpehsEngine/Graphics/Internal/Batch.h"

#include "SpehsEngine/Core/BitwiseOperations.h"
#include "SpehsEngine/Core/SE_Assert.h"
#include "SpehsEngine/Graphics/Internal/InternalUtilities.h"
#include "SpehsEngine/Graphics/Internal/DefaultUniforms.h"


namespace se
{
	namespace graphics
	{
		static constexpr uint32_t MAX_BUFFER_SIZE = 65536;

		Batch::Batch(const RenderInfo _renderInfo)
			: renderInfo(_renderInfo)
		{
			// TODO: Test BGFX_BUFFER_ALLOW_RESIZE?
			vertexBufferHandle = bgfx::createDynamicVertexBuffer(MAX_BUFFER_SIZE, findVertexLayout(renderInfo.attributes));
			indexBufferHandle = bgfx::createDynamicIndexBuffer(MAX_BUFFER_SIZE);
			vertices.setAttributes(renderInfo.attributes);
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
			return renderInfo == _renderInfo;
		}
		bool Batch::check(const size_t numVertices, const size_t numIndices) const
		{
			if (numVertices > MAX_BUFFER_SIZE || numIndices > MAX_BUFFER_SIZE)
				log::error("Max batch size exceeded!");
			return (MAX_BUFFER_SIZE - vertices.size()) >= numVertices &&
				   (MAX_BUFFER_SIZE - indices.size()) >= numIndices;
		}

		[[nodiscard]] const BatchPosition& Batch::add(const VertexBuffer& _vertices, const std::vector<IndexType>& _indices)
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

			vertices.pushBack(_vertices);

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

			vertices.erase(_positionInBatch.verticesStart, _positionInBatch.verticesEnd);
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

		void Batch::updateVertices(const BatchPosition& _positionInBatch, const VertexBuffer& _vertices,
								   const glm::mat4& _transformMatrix, const glm::mat4& _normalMatrix)
		{
			se_assert(_vertices.size() == (_positionInBatch.verticesEnd - _positionInBatch.verticesStart));
			se_assert(vertices.getAttributes() == _vertices.getAttributes());
			glm::vec4 newVertex;
			for (size_t i = 0; i < _vertices.size(); i++)
			{
				if (checkBit(renderInfo.attributes, VertexAttributeFlag::Position))
				{
					newVertex = _transformMatrix * glm::vec4(_vertices.get<Position>(i), 1.0f);
					vertices.get<Position>(i + _positionInBatch.verticesStart) = newVertex;
				}
				if (checkBit(renderInfo.attributes, VertexAttributeFlag::Normal))
				{
					newVertex = _normalMatrix * glm::vec4(_vertices.get<Normal>(i), 1.0f);
					newVertex = glm::normalize(newVertex);
					vertices.get<Normal>(i + _positionInBatch.verticesStart) = newVertex;
				}
				if (checkBit(renderInfo.attributes, VertexAttributeFlag::Tangent))
				{
					newVertex = _normalMatrix * glm::vec4(_vertices.get<Tangent>(i), 1.0f);
					newVertex = glm::normalize(newVertex);
					vertices.get<Tangent>(i + _positionInBatch.verticesStart) = newVertex;
				}
				if (checkBit(renderInfo.attributes, VertexAttributeFlag::Bitangent))
				{
					newVertex = _normalMatrix * glm::vec4(_vertices.get<Bitangent>(i), 1.0f);
					newVertex = glm::normalize(newVertex);
					vertices.get<Bitangent>(i + _positionInBatch.verticesStart) = newVertex;
				}
				if (checkBit(renderInfo.attributes, VertexAttributeFlag::Color0))
				{
					vertices.get<Color0>(i + _positionInBatch.verticesStart) = _vertices.get<Color0>(i);
				}
				if (checkBit(renderInfo.attributes, VertexAttributeFlag::Color1))
				{
					vertices.get<Color1>(i + _positionInBatch.verticesStart) = _vertices.get<Color1>(i);
				}
				if (checkBit(renderInfo.attributes, VertexAttributeFlag::Color2))
				{
					vertices.get<Color2>(i + _positionInBatch.verticesStart) = _vertices.get<Color2>(i);
				}
				if (checkBit(renderInfo.attributes, VertexAttributeFlag::Color3))
				{
					vertices.get<Color3>(i + _positionInBatch.verticesStart) = _vertices.get<Color3>(i);
				}
				if (checkBit(renderInfo.attributes, VertexAttributeFlag::Weight))
				{
					vertices.get<Weight>(i + _positionInBatch.verticesStart) = _vertices.get<Weight>(i);
				}
				if (checkBit(renderInfo.attributes, VertexAttributeFlag::TexCoord0))
				{
					vertices.get<TexCoord0>(i + _positionInBatch.verticesStart) = _vertices.get<TexCoord0>(i);
				}
				if (checkBit(renderInfo.attributes, VertexAttributeFlag::TexCoord1))
				{
					vertices.get<TexCoord1>(i + _positionInBatch.verticesStart) = _vertices.get<TexCoord1>(i);
				}
				if (checkBit(renderInfo.attributes, VertexAttributeFlag::TexCoord2))
				{
					vertices.get<TexCoord2>(i + _positionInBatch.verticesStart) = _vertices.get<TexCoord2>(i);
				}
				if (checkBit(renderInfo.attributes, VertexAttributeFlag::TexCoord3))
				{
					vertices.get<TexCoord3>(i + _positionInBatch.verticesStart) = _vertices.get<TexCoord3>(i);
				}
				if (checkBit(renderInfo.attributes, VertexAttributeFlag::TexCoord4))
				{
					vertices.get<TexCoord4>(i + _positionInBatch.verticesStart) = _vertices.get<TexCoord4>(i);
				}
				if (checkBit(renderInfo.attributes, VertexAttributeFlag::TexCoord5))
				{
					vertices.get<TexCoord5>(i + _positionInBatch.verticesStart) = _vertices.get<TexCoord5>(i);
				}
				if (checkBit(renderInfo.attributes, VertexAttributeFlag::TexCoord6))
				{
					vertices.get<TexCoord6>(i + _positionInBatch.verticesStart) = _vertices.get<TexCoord6>(i);
				}
				if (checkBit(renderInfo.attributes, VertexAttributeFlag::TexCoord7))
				{
					vertices.get<TexCoord7>(i + _positionInBatch.verticesStart) = _vertices.get<TexCoord7>(i);
				}
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

			// TODO: Optimize?
			static const glm::mat4 identity = glm::mat4(1.0f);
			bgfx::setTransform(reinterpret_cast<const void*>(&identity));
			_renderContext.defaultUniforms->setNormalMatrix(identity);

			renderInfo.material->update();

			bgfx::setIndexBuffer(indexBufferHandle, 0, (uint32_t)indices.size());
			bgfx::setVertexBuffer(0, vertexBufferHandle);

			applyRenderState(renderInfo, _renderContext);
			bgfx::submit(_renderContext.currentViewId, renderInfo.material->getShader()->programHandle);
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
				bufferMemory = bgfx::copy(&indices[0], uint32_t(indices.size() * sizeof(indices[0])));
				bgfx::update(indexBufferHandle, 0, bufferMemory);
				needsIndexBufferUpdate = false;
			}
			if (needsVertexBufferUpdate)
			{
				bufferMemory = bgfx::copy(vertices.data(), uint32_t(vertices.bytes()));
				bgfx::update(vertexBufferHandle, 0, bufferMemory);
				needsVertexBufferUpdate = false;
			}
		}
	}
}
