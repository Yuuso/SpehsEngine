
#include "Batch.h"
#include "Console.h"
#include "Primitive.h"
#include "Mesh.h"
#include "BatchManager.h"
#include "ShaderManager.h"
#include "OpenGLError.h"
#include "Vertex.h"
#include "Camera2D.h"
#include "Time.h"

#include <glm/mat4x4.hpp>
#include <GL/glew.h>

#define DEFAULT_MAX_BATCH_SIZE 4096


namespace spehs
{
	int getIndexMultiplier(const GLenum &_drawMode)
	{
		switch (_drawMode)
		{
		case TRIANGLE:
			return (DEFAULT_MAX_BATCH_SIZE - 2) * 3;
			break;
		default:
			return DEFAULT_MAX_BATCH_SIZE;
			break;
		}
	}

	//PRIMITIVE BATCH
	PrimitiveBatch::PrimitiveBatch()
	{
		vertexArrayObjectID = 0;
		vertexBufferID = 0;
		indexBufferID = 0;

		blending = false;
		cameraMatrixState = false;
		shaderIndex = -1;
		textureDataID = 0;
		drawMode = 0;
		lineWidth = 0.0f;
		totalNumvertices = 0;
		priority = 0;

		initBuffers();
	}
	PrimitiveBatch::PrimitiveBatch(const bool _cameraMatrixState, const int16_t &_priority, const bool _blending, const int &_shaderIndex, const GLuint &_textureDataID, const GLenum &_drawMode, float _lineWidth)
	{
		vertexArrayObjectID = 0;
		vertexBufferID = 0;
		indexBufferID = 0;

		cameraMatrixState = _cameraMatrixState;
		blending = _blending;
		shaderIndex = _shaderIndex;
		textureDataID = _textureDataID;
		drawMode = _drawMode;
		lineWidth = _lineWidth;
		priority = _priority;

		totalNumvertices = 0;

		vertices.reserve(DEFAULT_MAX_BATCH_SIZE);
		indices.reserve(getIndexMultiplier(drawMode));

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

		checkOpenGLErrors(__FILE__, __LINE__);
	}


	bool PrimitiveBatch::operator==(const Primitive &_primitive)
	{
		if (shaderIndex != _primitive.shaderIndex ||
			textureDataID != _primitive.textureDataID ||
			drawMode != _primitive.drawMode ||
			priority != _primitive.planeDepth ||
			blending != _primitive.blending ||
			cameraMatrixState != _primitive.cameraMatrixState)
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
		if (totalNumvertices == 0)
			return false;
		
		updateBuffers();

		//Blending
		if (blending)
			glEnable(GL_BLEND);
		else
			glDisable(GL_BLEND);

		shaderManager->use(shaderIndex);

		//Texture
		if (textureDataID)
			shaderManager->getShader(shaderIndex)->uniforms->textureDataID = textureDataID;

		//Camera Matrix
		if (cameraMatrixState)
			shaderManager->getShader(shaderIndex)->uniforms->cameraMatrix = *spehs::getActiveBatchManager()->getCamera2D()->projectionMatrix;
		else
			shaderManager->getShader(shaderIndex)->uniforms->cameraMatrix = spehs::getActiveBatchManager()->getCamera2D()->staticMatrix;

		//Uniforms
		shaderManager->setUniforms(shaderIndex);

		//Draw
		glBindVertexArray(vertexArrayObjectID);
		if (lineWidth != 0.0f)
		{
			glLineWidth(lineWidth);
		}
		glDrawElements(drawMode, indices.size(), GL_UNSIGNED_SHORT, (GLvoid*) NULL);
		glBindVertexArray(0);

		checkOpenGLErrors(__FILE__, __LINE__);

		shaderManager->unuse(shaderIndex);

		drawCalls++;
		vertexDrawCount += totalNumvertices;

		//Clean up
		vertices.clear();
		indices.clear();
		totalNumvertices = 0;
		return true;
	}

	void PrimitiveBatch::push(Primitive* _primitive)
	{
		setIndices(_primitive->numVertices); //Set indices before vertices!
		//MEMCPY WAY
		//vertices.resize(vertices.size() + _primitive->numVertices);
		//memcpy(&vertices[totalNumvertices], &_primitive->worldVertexArray[0], sizeof(Vertex) * _primitive->numVertices);
		//PUSH_BACK WAY
		for (unsigned i = 0; i < _primitive->numVertices; i++)
		{
			vertices.push_back(_primitive->worldVertexArray[i]);
		}
		totalNumvertices += _primitive->numVertices;
	}
	
	//Private:
	bool PrimitiveBatch::isEnoughRoom(const unsigned int &_numVertices)
	{
		if (_numVertices > DEFAULT_MAX_BATCH_SIZE)
			exceptions::fatalError("The number of vertices in a primitive exceeds the max amount allowed in the batch!");
		return (totalNumvertices + _numVertices) <= DEFAULT_MAX_BATCH_SIZE;
	}

	void PrimitiveBatch::initBuffers()
	{
		//Generate VAO
		glGenVertexArrays(1, &vertexArrayObjectID);
		glBindVertexArray(vertexArrayObjectID);
		//Generate buffers
		glGenBuffers(1, &vertexBufferID);
		glGenBuffers(1, &indexBufferID);

		checkOpenGLErrors(__FILE__, __LINE__);

		//Bind buffers
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(spehs::Vertex) * DEFAULT_MAX_BATCH_SIZE, nullptr, GL_STREAM_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * getIndexMultiplier(drawMode), nullptr, GL_STREAM_DRAW);

		checkOpenGLErrors(__FILE__, __LINE__);

		//Attributes
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		if (textureDataID)
			glEnableVertexAttribArray(2);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(spehs::Vertex), reinterpret_cast<void*>(offsetof(spehs::Vertex, position)));
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(spehs::Vertex), reinterpret_cast<void*>(offsetof(spehs::Vertex, color)));
		if (textureDataID)
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(spehs::Vertex), reinterpret_cast<void*>(offsetof(spehs::Vertex, uv)));

		//Unbind
		glBindVertexArray(0);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		if (textureDataID)
			glDisableVertexAttribArray(2);
		
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		checkOpenGLErrors(__FILE__, __LINE__);
	}

	void PrimitiveBatch::updateBuffers()
	{
		glBindVertexArray(vertexArrayObjectID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

		//Sent data to GPU
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(spehs::Vertex) * vertices.size(), &vertices[0]);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(GLushort) * indices.size(), &indices[0]);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		checkOpenGLErrors(__FILE__, __LINE__);
	}

	void PrimitiveBatch::setIndices(const unsigned int &_numVertices)
	{
		static unsigned int currentIndex;
		static unsigned int numIndices;
		currentIndex = totalNumvertices;
		numIndices = indices.size();
		switch (drawMode)
		{
		case UNDEFINED:
			exceptions::fatalError(std::to_string(drawMode) + " - Batch's DrawMode is UNDEFINED!");
			break;

		case POINT:
			if (cameraMatrixState)
				glPointSize(getActiveBatchManager()->getCamera2D()->scale * 2);
			else
				glPointSize(1.0f);
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
			for (unsigned i = 1; indices.size() < (numIndices + ((_numVertices - 2) * 3));)
			{
				indices.push_back((GLushort) currentIndex);
				indices.push_back((GLushort) (currentIndex + i++));
				indices.push_back((GLushort) (currentIndex + i));
			}
			break;

		default:
			exceptions::fatalError(std::to_string(drawMode) + " - Draw mode is either undefined or is not supported by the engine at the moment!");
			break;
		}
	}


	//MESH BATCH
	MeshBatch::MeshBatch()
	{
		vertexArrayObjectID = 0;
		vertexBufferID = 0;
		indexBufferID = 0;
		normalBufferID = 0;

		shaderIndex = 0;
		totalNumvertices = 0;
		textureDataID = 0;
		drawMode = UNDEFINED;
		lineWidth = 0.0f;

		initBuffers();
	}
	MeshBatch::MeshBatch(const int &_shader, const GLuint &_textureID, const GLenum &_drawMode, const float &_lineWidth)
	{
		vertexArrayObjectID = 0;
		vertexBufferID = 0;
		indexBufferID = 0;
		normalBufferID = 0;

		totalNumvertices = 0;

		shaderIndex = _shader;
		textureDataID = _textureID;
		drawMode = _drawMode;
		lineWidth = _lineWidth;

		vertices.reserve(DEFAULT_MAX_BATCH_SIZE);
		normals.reserve(DEFAULT_MAX_BATCH_SIZE);
		indices.reserve(getIndexMultiplier(drawMode));

		initBuffers();
	}
	MeshBatch::~MeshBatch()
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
		if (normalBufferID != 0)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glDeleteBuffers(1, &normalBufferID);
		}
		if (vertexArrayObjectID != 0)
		{
			glBindVertexArray(0);
			glDeleteVertexArrays(1, &vertexArrayObjectID);
		}

		checkOpenGLErrors(__FILE__, __LINE__);
	}


	bool MeshBatch::operator==(const Mesh &_mesh)
	{
		if (shaderIndex != _mesh.shaderIndex ||
			textureDataID != _mesh.textureDataID ||
			drawMode != _mesh.drawMode)
		{
			return false;
		}
		if (drawMode == LINE ||
			drawMode == LINE_LOOP ||
			drawMode == LINE_STRIP ||
			drawMode == LINE_STRIP_ADJACENCY ||
			drawMode == LINE_ADJACENCY)
		{
			if (lineWidth != _mesh.lineWidth)
			{
				return false;
			}
		}
		if (!isEnoughRoom(_mesh.numVertices))
		{
			return false;
		}
		return true;
	}


	bool MeshBatch::render()
	{
		if (totalNumvertices == 0)
			return false;

		updateBuffers();

		shaderManager->use(shaderIndex);

		//Texture
		if (textureDataID)
			shaderManager->getShader(shaderIndex)->uniforms->textureDataID = textureDataID;

		//TODO:Camera Matrix!

		//Uniforms
		shaderManager->setUniforms(shaderIndex);

		//Draw
		glBindVertexArray(vertexArrayObjectID);
		if (lineWidth != 0.0f)
		{
			glLineWidth(lineWidth);
		}
		glDrawElements(drawMode, indices.size(), GL_UNSIGNED_SHORT, (GLvoid*) NULL);
		glBindVertexArray(0);

		checkOpenGLErrors(__FILE__, __LINE__);

		shaderManager->unuse(shaderIndex);

		drawCalls++;
		vertexDrawCount += totalNumvertices;

		//Clean up
		vertices.clear();
		indices.clear();
		normals.clear();
		totalNumvertices = 0;
		return true;
	}

	void MeshBatch::push(Mesh* _mesh)
	{
		//INDICES
		for (unsigned i = 0; i < _mesh->elementArray.size(); i++)
		{
			indices.push_back(_mesh->elementArray[i]);
		}
		//VERTICES
		for (unsigned i = 0; i < _mesh->numVertices; i++)
		{
			vertices.push_back(_mesh->worldVertexArray[i]);
		}
		totalNumvertices += _mesh->numVertices;
		//NORMALS
		for (unsigned i = 0; i < _mesh->normalArray.size(); i++)
		{
			normals.push_back(_mesh->normalArray[i]);
		}
	}

	//Private:
	bool MeshBatch::isEnoughRoom(const unsigned int &_numVertices)
	{
		if (_numVertices > DEFAULT_MAX_BATCH_SIZE)
			exceptions::fatalError("The number of vertices in a mesh exceeds the max amount allowed in the batch!");
		return (totalNumvertices + _numVertices) <= DEFAULT_MAX_BATCH_SIZE;
	}

	void MeshBatch::initBuffers()
	{
		//Generate VAO
		glGenVertexArrays(1, &vertexArrayObjectID);
		glBindVertexArray(vertexArrayObjectID);
		//Generate buffers
		glGenBuffers(1, &vertexBufferID);
		glGenBuffers(1, &indexBufferID);
		glGenBuffers(1, &normalBufferID);

		checkOpenGLErrors(__FILE__, __LINE__);

		//Bind buffers
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(spehs::Vertex) * DEFAULT_MAX_BATCH_SIZE, nullptr, GL_STREAM_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * getIndexMultiplier(drawMode), nullptr, GL_STREAM_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, normalBufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::vec3) * DEFAULT_MAX_BATCH_SIZE, nullptr, GL_STREAM_DRAW);

		checkOpenGLErrors(__FILE__, __LINE__);

		//Attributes
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		if (textureDataID)
			glEnableVertexAttribArray(2);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(spehs::Vertex), reinterpret_cast<void*>(offsetof(spehs::Vertex, position)));
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(spehs::Vertex), reinterpret_cast<void*>(offsetof(spehs::Vertex, color)));
		if (textureDataID)
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(spehs::Vertex), reinterpret_cast<void*>(offsetof(spehs::Vertex, uv)));

		//Unbind
		glBindVertexArray(0);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		if (textureDataID)
			glDisableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		checkOpenGLErrors(__FILE__, __LINE__);
	}

	void MeshBatch::updateBuffers()
	{
		glBindVertexArray(vertexArrayObjectID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

		//Sent data to GPU
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(spehs::Vertex) * vertices.size(), &vertices[0]);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(GLushort) * indices.size(), &indices[0]);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, normalBufferID);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(glm::vec3) * normals.size(), &normals[0]);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		checkOpenGLErrors(__FILE__, __LINE__);
	}
}