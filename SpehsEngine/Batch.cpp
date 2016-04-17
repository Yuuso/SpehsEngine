
#include "Batch.h"
#include "Console.h"
#include "Primitive.h"
#include "Mesh.h"
#include "BatchManager.h"
#include "ShaderManager.h"
#include "OpenGLError.h"
#include "Vertex.h"
#include "Camera2D.h"
#include "Camera3D.h"
#include "Time.h"

#include <glm/mat4x4.hpp>
#include <GL/glew.h>
#include <algorithm>


namespace spehs
{
	int getIndexMultiplier(const GLenum &_drawMode, const unsigned int& _batchSize)
	{
		switch (_drawMode)
		{
		case TRIANGLE:
			return (_batchSize - 2) * 3;
			break;
		default:
			return _batchSize;
			break;
		}
	}

	//PRIMITIVE BATCH
#pragma region PRIMITIVE BATCH
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
		if (!isEnoughRoom(_primitive.worldVertexArray.size()))
		{
			return false;
		}
		return true;
	}


	bool PrimitiveBatch::render()
	{
		if (vertices.size() == 0)
			return false;
		
		updateBuffers();

		//Blending
		if (blending)
		{
			glEnable(GL_BLEND);
			glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
		}
		else
		{
			glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
		}

		shaderManager->use(shaderIndex);

		//Texture
		if (textureDataID)
		{
			shaderManager->getShader(shaderIndex)->uniforms->textureDataID = textureDataID;
			if (drawMode == POINT)
			{
				glEnable(GL_POINT_SPRITE);
				glEnable(GL_PROGRAM_POINT_SIZE);
			}
		}

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
		vertexDrawCount += vertices.size();

		//Clean up
		vertices.clear();
		indices.clear();
		return true;
	}

	void PrimitiveBatch::push(Primitive* _primitive)
	{
		//INDICES
		setIndices(_primitive->worldVertexArray.size());
		//VERTICES
		vertices.insert(vertices.end(), _primitive->worldVertexArray.begin(), _primitive->worldVertexArray.end());
	}
	
	//Private:
	bool PrimitiveBatch::isEnoughRoom(const unsigned int &_numVertices)
	{
		if (_numVertices > DEFAULT_MAX_BATCH_SIZE)
			exceptions::fatalError("The number of vertices in a primitive exceeds the max amount allowed in the batch!");
		return (vertices.size() + _numVertices) <= DEFAULT_MAX_BATCH_SIZE;
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
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(spehs::Vertex), reinterpret_cast<void*>(offsetof(spehs::Vertex, position)));
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(spehs::Vertex), reinterpret_cast<void*>(offsetof(spehs::Vertex, color)));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(spehs::Vertex), reinterpret_cast<void*>(offsetof(spehs::Vertex, uv)));

		//Unbind
		glBindVertexArray(0);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
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
		unsigned int currentIndex = (vertices.size());
		unsigned int numIndices = (indices.size());
		unsigned int index = (numIndices);
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
			indices.resize(indices.size() + _numVertices);
			for (unsigned i = numIndices; i < numIndices + _numVertices; i++)
			{
				indices[i] = (GLushort) currentIndex + (i - numIndices);
			}
			break;

		case TRIANGLE:
			/*
			Number of indices in a polygon:
			(NumberOfVertices - 2) * 3
			*/
			indices.resize(indices.size() + ((_numVertices - 2) * 3));
			for (unsigned i = 1; index < (numIndices + ((_numVertices - 2) * 3));)
			{
				indices[index++] = ((GLushort) currentIndex);
				indices[index++] = ((GLushort) (currentIndex + i++));
				indices[index++] = ((GLushort) (currentIndex + i));
			}
			break;

		default:
			exceptions::fatalError(std::to_string(drawMode) + " - Draw mode is either undefined or is not supported by the engine at the moment!");
			break;
		}
	}
#pragma endregion


	//MESH BATCH
#pragma region MESH BATCH
	MeshBatch::MeshBatch()
	{
		vertexArrayObjectID = 0;
		vertexBufferID = 0;
		indexBufferID = 0;

		blending = false;
		batchSize = DEFAULT_MAX_BATCH_SIZE;
		shaderIndex = 0;
		textureDataID = 0;
		drawMode = UNDEFINED;
		lineWidth = 0.0f;

		initBuffers();
	}
	MeshBatch::MeshBatch(const unsigned int& _batchSizeCheck, const int &_shader, const GLuint &_textureID, const GLenum &_drawMode, const bool _blending, const float &_lineWidth)
	{
		vertexArrayObjectID = 0;
		vertexBufferID = 0;
		indexBufferID = 0;

		if (_batchSizeCheck > DEFAULT_MAX_BATCH_SIZE)
			batchSize = _batchSizeCheck;
		else
			batchSize = DEFAULT_MAX_BATCH_SIZE;

		blending = _blending;
		shaderIndex = _shader;
		textureDataID = _textureID;
		drawMode = _drawMode;
		lineWidth = _lineWidth;

		vertices.reserve(batchSize);
		indices.reserve(batchSize * 4); //For mesh batch this is not the true max value

		initBuffers();
	}
	MeshBatch::~MeshBatch()
	{
		clearGPUBuffers();
	}


	bool MeshBatch::operator==(const Mesh &_mesh)
	{
		if (shaderIndex != _mesh.shaderIndex ||
			textureDataID != _mesh.textureDataID ||
			drawMode != _mesh.drawMode ||
			blending != _mesh.blending)
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
		if (!isEnoughRoom(_mesh.worldVertexArray.size()))
		{
			return false;
		}
		return true;
	}


	bool MeshBatch::render()
	{
		if (vertices.size() == 0)
			return false;

		if (!blending)
		{
			glDisable(GL_BLEND);
			glDepthMask(GL_TRUE);
			glEnable(GL_DEPTH_TEST);
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
			glEnable(GL_BLEND);
		}

		updateBuffers();

		shaderManager->use(shaderIndex);

		//Texture
		if (textureDataID)
		{
			shaderManager->getShader(shaderIndex)->uniforms->textureDataID = textureDataID;
			if (drawMode == POINT)
			{
				glEnable(GL_POINT_SPRITE);
				glEnable(GL_PROGRAM_POINT_SIZE);
			}
		}

		shaderManager->getShader(shaderIndex)->uniforms->cameraMatrix = *spehs::getActiveBatchManager()->getCamera3D()->cameraMatrix;

		//Uniforms
		shaderManager->setUniforms(shaderIndex);

		//Draw
		glBindVertexArray(vertexArrayObjectID);
		if (lineWidth != 0.0f)
		{
			glLineWidth(lineWidth);
		}
		glDrawElements(drawMode, indices.size(), GL_UNSIGNED_SHORT, reinterpret_cast<void*>(0));
		glBindVertexArray(0);

		checkOpenGLErrors(__FILE__, __LINE__);

		shaderManager->unuse(shaderIndex);

		drawCalls++;
		vertexDrawCount += vertices.size();

		//Clean up
		vertices.clear();
		indices.clear();
		return true;
	}

	void MeshBatch::push(Mesh* _mesh)
	{
		GLushort firstElement = vertices.size();
		unsigned int indSize = indices.size();
		//INDICES
		indices.insert(indices.end(), _mesh->elementArray.begin(), _mesh->elementArray.end());
		if (firstElement != 0)
			for (size_t i = indSize; i < indices.size(); i++)
			{
				indices[i] += firstElement;
			}
		//VERTICES
		vertices.insert(vertices.end(), _mesh->worldVertexArray.begin(), _mesh->worldVertexArray.end());
	}

	//Private:
	bool MeshBatch::isEnoughRoom(const unsigned int &_numVertices)
	{
		if (_numVertices > DEFAULT_MAX_BATCH_SIZE)
		{
			if (vertices.size() == 0) //If this is empty batch, resize it for the mesh
			{
				batchSize = _numVertices;
				clearGPUBuffers();
				initBuffers();
				return true;
			}
			return false;
		}
		return (vertices.size() + _numVertices) <= DEFAULT_MAX_BATCH_SIZE;
	}

	void MeshBatch::initBuffers()
	{
		clearGPUBuffers();
		//Generate VAO
		glGenVertexArrays(1, &vertexArrayObjectID);
		//Generate buffers
		glGenBuffers(1, &vertexBufferID);
		glGenBuffers(1, &indexBufferID);

		checkOpenGLErrors(__FILE__, __LINE__);

		//Bind buffers
		glBindVertexArray(vertexArrayObjectID);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(spehs::Vertex3D) * batchSize, nullptr, GL_STREAM_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * batchSize * 4, nullptr, GL_STREAM_DRAW);

		checkOpenGLErrors(__FILE__, __LINE__);

		//Attributes
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(spehs::Vertex3D), reinterpret_cast<void*>(offsetof(spehs::Vertex3D, position)));
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(spehs::Vertex3D), reinterpret_cast<void*>(offsetof(spehs::Vertex3D, color)));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(spehs::Vertex3D), reinterpret_cast<void*>(offsetof(spehs::Vertex3D, normal)));
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(spehs::Vertex3D), reinterpret_cast<void*>(offsetof(spehs::Vertex3D, uv)));

		//Unbind
		glBindVertexArray(0);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(3);

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
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(spehs::Vertex3D) * vertices.size(), vertices.data());
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(GLushort) * indices.size(), indices.data());

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		checkOpenGLErrors(__FILE__, __LINE__);
	}

	void MeshBatch::clearGPUBuffers()
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
#pragma endregion
}