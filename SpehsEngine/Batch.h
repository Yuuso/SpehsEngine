#pragma once

#include <vector>


typedef unsigned int GLenum;
typedef unsigned int GLuint;
typedef unsigned short GLushort;

namespace SpehsEngine
{
	class BatchRenderer;
	class BatchObject;
	
	//Batch holds a number of BatchObjects that are all rendered in the same draw call
	class Batch
	{
		friend class BatchRenderer;

	public:
		Batch();
		Batch(const bool _blending, const int &_shaderIndex, const GLuint &_textureDataID, const GLenum &_drawMode);
		~Batch();

		void render();
		void push(BatchObject* _batchObject);
		void remove(BatchObject* _batchObject);

	protected:
		void updateIndices(BatchObject* _batchObject);

		std::vector<BatchObject*> batchObjects;
		std::vector<GLushort> indices;

		GLuint vertexArrayObjectID;
		GLuint vertexBufferID;
		GLuint indexBufferID;
		GLuint normalBufferID;
		GLuint textureDataID;

		bool blending;
		int shaderIndex;
		GLuint textureDataID;
		GLenum drawMode;
	};
}
