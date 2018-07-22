
#pragma once

#include "SpehsEngine/Rendering/Vertex3D.h"

#include <vector>
#include <utility>

typedef unsigned int GLenum;
typedef unsigned int GLuint;
typedef unsigned short GLushort;


namespace se
{
	namespace rendering
	{
		class Mesh;
		class BatchManager3D;
		struct BatchRenderResults;

		class MeshBatch
		{
		public:
			MeshBatch(BatchManager3D& _batchManager, const int _shaderIndex, const GLuint _textureDataID,
						const bool _depthTest, const bool _blending, const bool _backFaceCulling, const GLenum _drawMode);
			~MeshBatch();

			bool checkCompatibility(const Mesh& _mesh) const;
			bool checkSize(const size_t _numVertices, const size_t _numIndices) const;

			std::pair<size_t, size_t> push(const Mesh& _mesh);
			void remove(const std::pair<size_t, size_t> _index, std::pair<size_t, size_t> _size);
			void updateVertices(const size_t _index, const Mesh& _mesh);

			bool render(BatchRenderResults* results = nullptr);

			bool getBlending() const { return blending; }
			bool getDepthTest() const { return depthTest; }

		protected:
			void initBuffers();
			void updateBuffers();

		private:
			BatchManager3D& batchManager;

			std::vector<Vertex3D> vertices;
			std::vector<GLushort> indices;
			size_t vertexBufferSize = 0;
			size_t indexBufferSize = 0;
			bool needBufferUpdate = true;

			GLuint vertexArrayObjectID = 0;
			GLuint vertexBufferID = 0;
			GLuint indexBufferID = 0;

			const bool backFaceCulling;
			const bool blending;
			const bool depthTest;
			const size_t shaderIndex;
			const GLuint textureDataID;
			const GLenum usage;
			const GLenum drawMode;
		};
	}
}
