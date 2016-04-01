#pragma once

#include "Primitive.h"
#include "Vertex.h"

#include <glm/vec3.hpp>

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
	
	int getIndexMultiplier(const GLenum &_drawMode); //Calculate max number of indices


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
		bool isEnoughRoom(const unsigned int &_numVertices);
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
		MeshBatch(const int &_shader, const GLuint &_textureID, const GLenum &_drawMode, const float &_lineWidth = 0.0f);
		~MeshBatch();

		bool operator==(const Mesh &_mesh); //Checks if mesh is suitable for this batch

		void render();
		void push(Mesh* _mesh);

	private:
		bool isEnoughRoom(const unsigned int &_numVertices);
		void initBuffers();
		void updateBuffers();

		std::vector<Vertex> vertices;
		std::vector<GLushort> indices;
		std::vector<glm::vec3> normals;

		GLuint vertexArrayObjectID;
		GLuint vertexBufferID;
		GLuint indexBufferID;
		GLuint normalBufferID;

		int shaderIndex;
		unsigned int totalNumvertices;
		GLuint textureDataID;
		GLenum drawMode;
		float lineWidth;

	};
}
