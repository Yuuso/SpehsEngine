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
		~Batch();

		void render();
		void push(BatchObject* _batchObject);
		void remove(BatchObject* _batchObject);

	protected:
		void updateIndices();

		std::vector<BatchObject*> batchObjects;
		std::vector<GLushort> indices;

		int shaderIndex;
		GLuint textureDataID;
		GLenum drawMode;
	};
}
