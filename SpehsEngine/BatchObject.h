#pragma once

#include "Vertex.h"

#include <glm/mat4x4.hpp>


typedef unsigned int GLuint;

namespace SpehsEngine
{
	class Primitive;
	class Polygon;
	class Line;
	class Point;
	class Mesh;
	class Batch;

	//Base class for drawables: Primitives and Meshes
	class BatchObject
	{
		friend class Batch;

	public:
		BatchObject();

		virtual void updateVertices() = 0; //This is called automatically when rendering

		virtual Mesh* getMeshPtr(){ return nullptr; }
		virtual Primitive* getPrimitivePtr(){ return nullptr; }
		virtual Polygon* getPolygonPtr(){ return nullptr; }
		virtual Line* getLinePtr(){ return nullptr; }
		virtual Point* getPointPtr(){ return nullptr; }

		/*
		IMPORTANT NOTE:
		All drawables have a two-step deletion.
		When user calls destroy, BatchObject is marked to be removed by Batch. If BatchObject is not in a Batch it is deleted.
		Batch checks if user has asked for deletion and calls delete -> which will actually delete the BatchObject.
		(Destructor is protected to prevent user from deleting the BatchObject.)
		*/
		void destroy();

		//Setters
		void setRenderState(const bool _newState);
		void setShader(const int &_newShaderIndex);

		//Getters
		int getShaderIndex(){ return shaderIndex; }

		//Public Variables
		Vertex* worldVertexArray; //Transformed vertices
		GLuint numVertices;

	protected:
		virtual ~BatchObject();

		bool readyForDelete;
		bool renderState; //Whether BatchObject is rendered or not
		bool useColor; //When true: Colors are sent as uniform
		int shaderIndex;
		GLuint textureDataID;
		glm::mat4 scaledMatrix;
		glm::mat4 scaledRotatedMatrix;

		Position position;
		Vertex* vertexArray; //Original vertices
		Batch* batch;
	};
}