#pragma once

#include "Primitive.h"

#include <vector>
#include <stdint.h>


typedef unsigned int GLenum;
typedef unsigned int GLuint;
typedef unsigned short GLushort;
typedef int16_t PlaneDepth;

namespace spehs
{
	class BatchManager;
	class Mesh;
	class Vertex;
	
	//Batch for Primitives
	class PrimitiveBatch
	{
		friend class BatchManager;

	public:
		PrimitiveBatch();
		PrimitiveBatch(const bool _cameraMatrixState, const PlaneDepth &_priority, const bool _blending, const int &_shaderIndex, const GLuint &_textureDataID, const GLenum &_drawMode, float _lineWidth = 0.0f);
		~PrimitiveBatch();

		bool operator==(const Primitive &_primitive); //Checks if primitive is suitable for this batch

		bool render(); //Returns false if the batch is empty
		void push(Primitive* _primitive);

		PlaneDepth getPriority() const{ return priority; }

	protected:
		bool isEnoughRoom(unsigned int _numVertices);
		void initBuffers();
		void updateBuffers();
		void setIndices(const unsigned int &_numVertices);

		std::vector<Vertex> vertices;
		std::vector<GLushort> indices;

		GLuint vertexArrayObjectID;
		GLuint vertexBufferID;
		GLuint indexBufferID;

		bool blending;
		bool cameraMatrixState;
		int shaderIndex;
		unsigned int totalNumvertices;
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
		~MeshBatch();

		bool operator==(const Mesh &_primitive); //Checks if mesh is suitable for this batch

		void render();
		void push(Mesh* _batchObject);

	private:
		bool isEnoughRoom(unsigned int _numVertices);
		void initBuffers();

		
	};
}
