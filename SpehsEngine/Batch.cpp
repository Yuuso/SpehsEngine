
#include "Batch.h"
#include "Console.h"
#include "Primitive.h"
#include "Mesh.h"
#include "BatchManager.h"
#include "ShaderManager.h"
#include "OpenGLError.h"
#include "Vertex.h"

#include <GL/glew.h>


namespace spehs
{
	PrimitiveBatch::PrimitiveBatch()
	{
		vertexArrayObjectID = 0;
		vertexBufferID = 0;
		indexBufferID = 0;

		blending = false;
		shaderIndex = -1;
		textureDataID = 0;
		drawMode = 0;
		lineWidth = 0.0f;
		totalNumvertices = 0;
		priority = 0;
	}
	PrimitiveBatch::PrimitiveBatch(const int16_t &_priority, const bool _blending, const int &_shaderIndex, const GLuint &_textureDataID, const GLenum &_drawMode, float _lineWidth)
	{
		vertexArrayObjectID = 0;
		vertexBufferID = 0;
		indexBufferID = 0;

		blending = _blending;
		shaderIndex = _shaderIndex;
		textureDataID = _textureDataID;
		drawMode = _drawMode;
		lineWidth = _lineWidth;
		priority = _priority;

		totalNumvertices = 0;

		vertices.reserve(MAX_BATCH_DEFAULT_SIZE);

		initBuffers();
	}
	PrimitiveBatch::~PrimitiveBatch()
	{
		if (vertexBufferID != 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glDeleteBuffers(1, &vertexBufferID);
		}
		if (indexBufferID != 0)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glDeleteBuffers(1, &indexBufferID);
		}
		if (vertexArrayObjectID != 0)
		{
			glBindVertexArray(0);
			glDeleteVertexArrays(1, &vertexArrayObjectID);
		}
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
		if (drawMode == LINE ||
			drawMode == LINE_LOOP ||
			drawMode == LINE_STRIP ||
			drawMode == LINE_STRIP_ADJACENCY ||
			drawMode == LINE_ADJACENCY)
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
		
		updateBuffers();

		if (blending)
		{
			glEnable(GL_BLEND);
		}
		else
		{
			glDisable(GL_BLEND);
		}
		shaderManager->use(shaderIndex);
		if (textureDataID)
		{
			shaderManager->getShader(shaderIndex)->uniforms->textureDataID = textureDataID;
		}
		shaderManager->setUniforms(shaderIndex);

		glBindVertexArray(vertexArrayObjectID);
		if (lineWidth != 0.0f)
		{
			glLineWidth(lineWidth);
		}
		glDrawElements(drawMode, indices.size(), GL_UNSIGNED_SHORT, (GLvoid*) 0);
		glBindVertexArray(0);

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
		//Generate VAO
		glGenVertexArrays(1, &vertexArrayObjectID);
		glBindVertexArray(vertexArrayObjectID);
		//Generate buffers
		glGenBuffers(1, &vertexBufferID);
		glGenBuffers(1, &indexBufferID);

		//Bind buffers
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * MAX_BATCH_DEFAULT_SIZE, nullptr, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * MAX_BATCH_DEFAULT_SIZE, nullptr, GL_DYNAMIC_DRAW);

		//Attributes
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, position));
		glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, color));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, uv));

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		
		//Unbind
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void PrimitiveBatch::updateBuffers()
	{
		glBindVertexArray(vertexArrayObjectID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

		//Sent data to GPU
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * totalNumvertices, &vertices);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(GLushort) * indices.size(), &indices);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void PrimitiveBatch::setIndices(const unsigned int &_numVertices)
	{
		static unsigned int currentIndex;
		currentIndex = indices.size();
		switch (drawMode)
		{
		case UNDEFINED:
			exceptions::fatalError("Draw mode UNDEFINED for batch!");
			break;

		case POINT:
		case LINE:
		case LINE_LOOP:
			for (unsigned i = currentIndex; i < currentIndex + _numVertices; i++)
			{
				indices.push_back((GLushort) i);
			}
			break;

		case TRIANGLE:
			/*
			Number of indices in a polygon:
			(NumberOfVertices - 2) * 3
			*/
			for (unsigned i = 1; indices.size() < currentIndex + ((_numVertices - 2) * 3);)
			{
				indices.push_back((GLushort) currentIndex);
				indices.push_back((GLushort) (currentIndex + i));
				indices.push_back((GLushort) (currentIndex + ++i));
			}//not 100% sure if this is correct yet, see after testing
			break;

		default:
			exceptions::fatalError("Draw mode is either undefined or is not supported by the engine at the moment! DrawMode: " + drawMode);
			break;
		}
	}
}