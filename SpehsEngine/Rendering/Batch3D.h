
#pragma once

#include "SpehsEngine/Rendering/BatchRenderResults.h"
#include "SpehsEngine/Rendering/Vertex3D.h"

#include <vector>
#include <stdint.h>

#define DEFAULT_MAX_BATCH_SIZE 4096

typedef unsigned int GLenum;
typedef unsigned int GLuint;
typedef unsigned short GLushort;


namespace se
{
	class Mesh;
	class BatchManager3D;

	class MeshBatch
	{
	public:
		MeshBatch(BatchManager3D& _batchManager, const int _shaderIndex, const GLuint _textureDataID, const bool _depthTest, const bool _blending, const bool _backFaceCulling);
		~MeshBatch();

		bool check(const Mesh& _mesh);

		bool render(BatchRenderResults* results = nullptr);
		void push(Mesh* _mesh);

		bool getBlending() const { return blending; }
		bool getDepthTest() const { return depthTest; }

	protected:
		bool isEnoughRoom(const size_t _numVertices);
		void initBuffers();
		void updateBuffers();
		void setIndices(const size_t _numVertices);

	private:
		BatchManager3D& batchManager;

		std::vector<se::Vertex3D> vertices;
		std::vector<GLushort> indices;

		GLuint vertexArrayObjectID;
		GLuint vertexBufferID;
		GLuint indexBufferID;

		bool backFaceCulling;
		bool blending;
		bool depthTest;
		size_t shaderIndex;
		size_t batchSize;
		size_t indexSize;
		GLuint textureDataID;
	};
}
