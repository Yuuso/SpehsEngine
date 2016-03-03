
#include "Batch.h"
#include "Console.h"
#include "Primitive.h"
#include "Mesh.h"
#include "BatchManager.h"
#include "ShaderManager.h"
#include "OpenGLError.h"

#include <GL/glew.h>


namespace SpehsEngine
{
	PrimitiveBatch::PrimitiveBatch()
	{
		shaderIndex = -1;
		textureDataID = 0;
		drawMode = 0;
		lineWidth = 0.0f;
		totalNumvertices = 0;
	}
	PrimitiveBatch::PrimitiveBatch(const PlaneDepth &_priority, const bool _blending, const int &_shaderIndex, const GLuint &_textureDataID, const GLenum &_drawMode, float _lineWidth = 0.0f)
	{
		vertexArrayObjectID = 0;
		vertexBufferID = 0;
		indexBufferID = 0;

		blending = _blending;
		shaderIndex = _shaderIndex;
		textureDataID = _textureDataID;
		drawMode = _drawMode;
		lineWidth = _lineWidth;
		totalNumvertices = 0;
		priority = _priority;

		vertices.reserve(MAX_BATCH_DEFAULT_SIZE);

		initBuffers();
	}
	PrimitiveBatch::~PrimitiveBatch()
	{
	}


	bool PrimitiveBatch::operator==(const Primitive &_primitive)
	{
		if (shaderIndex != _primitive.shaderIndex ||
			textureDataID != _primitive.textureDataID ||
			drawMode != _primitive.drawMode ||
			priority != _primitive.planeDepth ||
			blending != _primitive.blending)
		{
			return false;
		}
		if (drawMode == GL_LINES) //TODO: Add the rest of the lineWidth using drawModes
		{
			if (lineWidth != _primitive.lineWidth)
			{
				return false;
			}
		}
		if (!isEnoughRoom(_primitive.numVertices))
		{
			return false;
		}
		return true;
	}


	bool PrimitiveBatch::render()
	{
		if (vertices.size() == 0)
		{
			return false;
		}

		//RENDERING

		vertices.clear();
		indices.clear();
		return true;
	}

	void PrimitiveBatch::push(Primitive* _primitive)
	{
		for (unsigned i = 0; i < _primitive->numVertices; i++)
		{
			vertices.push_back(&_primitive->vertexArray[i]);
		}
		setIndices(_primitive->numVertices);
		totalNumvertices += _primitive->numVertices;
	}
	
	bool PrimitiveBatch::isEnoughRoom(unsigned int _numVertices)
	{
		return (totalNumvertices + _numVertices) < MAX_BATCH_DEFAULT_SIZE;
	}

	void PrimitiveBatch::initBuffers()
	{
		glGenVertexArrays(1, &vertexArrayObjectID);
		glBindVertexArray(vertexArrayObjectID);
		glGenBuffers(1, &vertexBufferID);
		glGenBuffers(1, &indexBufferID);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * MAX_BATCH_DEFAULT_SIZE, nullptr, GL_STREAM_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * MAX_BATCH_DEFAULT_SIZE, nullptr, GL_STREAM_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);
		glBindVertexArray(0);
	}

	void PrimitiveBatch::setIndices(const unsigned int &_numVertices)
	{
		//INDICES
	}
}