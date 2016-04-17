#pragma once

#include "Primitive.h"
#include "Vertex.h"

#include <glm/vec3.hpp>

#include <vector>
#include <stdint.h>

#define DEFAULT_MAX_BATCH_SIZE 4096

typedef unsigned int GLenum;
typedef unsigned int GLuint;
typedef unsigned short GLushort;

typedef int16_t PlaneDepth;


namespace spehs
{
	class BatchManager;
	class Mesh;
	
	int getIndexMultiplier(const GLenum &_drawMode, const unsigned int& _batchSize = DEFAULT_MAX_BATCH_SIZE); //Calculate max number of indices


	//Batch for Primitives
	class PrimitiveBatch
	{
		friend class BatchManager;

	public:
		PrimitiveBatch();
		PrimitiveBatch(const bool _cameraMatrixState, const PlaneDepth &_priority, const bool _blending, const int &_shaderIndex, const GLuint &_textureDataID, const GLenum &_drawMode, float _lineWidth);
		~PrimitiveBatch();

		bool operator==(const Primitive &_primitive); //Checks if primitive is suitable for this batch

		bool render(); //Returns false if the batch is empty
		void push(Primitive* _primitive);

		PlaneDepth getPriority() const{ return priority; }

	protected:
		bool isEnoughRoom(const unsigned int &_numVertices);
		void initBuffers();
		void updateBuffers();
		void setIndices(const unsigned int &_numVertices);

		std::vector<spehs::Vertex> vertices;
		std::vector<GLushort> indices;

		GLuint vertexArrayObjectID;
		GLuint vertexBufferID;
		GLuint indexBufferID;

		bool blending;
		bool cameraMatrixState;
		int shaderIndex;
		GLuint textureDataID;
		GLenum drawMode;
		float lineWidth;
		PlaneDepth priority;
	};


	//Batch for Meshes
	class MeshBatch
	{
		friend class BatchManager;

	public:
		MeshBatch();
		MeshBatch(const unsigned int& _batchSizeCheck, const int &_shader, const GLuint &_textureID, const GLenum &_drawMode, const bool _blending, const float &_lineWidth);
		~MeshBatch();

		bool operator==(const Mesh &_mesh); //Checks if mesh is suitable for this batch

		bool render(); //Returns false if the batch is empty
		void push(Mesh* _mesh);

	private:
		bool isEnoughRoom(const unsigned int &_numVertices);
		void initBuffers();
		void updateBuffers();
		void clearGPUBuffers();

		std::vector<spehs::Vertex3D> vertices;
		std::vector<GLushort> indices;

		GLuint vertexArrayObjectID;
		GLuint vertexBufferID;
		GLuint indexBufferID;

		bool blending;
		int shaderIndex;
		unsigned int batchSize;
		GLuint textureDataID;
		GLenum drawMode;
		float lineWidth;

	};
}
