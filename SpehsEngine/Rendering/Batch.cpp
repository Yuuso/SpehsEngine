
#include "SpehsEngine/Rendering/Batch.h"
#include "SpehsEngine/Rendering/Console.h"
#include "SpehsEngine/Rendering/Primitive.h"
#include "SpehsEngine/Rendering/BatchManager.h"
#include "SpehsEngine/Rendering/ShaderManager.h"
#include "SpehsEngine/Rendering/GLSLProgram.h"
#include "SpehsEngine/Input/OpenGLError.h"
#include "SpehsEngine/Core/Vertex.h"
#include "SpehsEngine/Rendering/Camera2D.h"
#include "SpehsEngine/Core/Time.h"

#include <GL/glew.h>
#include <algorithm>
#include <atomic>


std::atomic<int> BatchAllocations;
std::atomic<int> BatchDeallocations;


namespace spehs
{
	Batch::Batch(const PlaneDepth &_priority, const int &_shaderIndex) : priority(_priority), shaderIndex(_shaderIndex)
	{
#ifdef _DEBUG
		BatchAllocations++;
#endif
	}
	Batch::~Batch()
	{
#ifdef _DEBUG
		BatchDeallocations++;
#endif
	}


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
	PrimitiveBatch::PrimitiveBatch(const bool _cameraMatrixState, const int16_t &_priority, const bool _blending, const int &_shaderIndex, const GLuint &_textureDataID, const GLenum &_drawMode, float _lineWidth)
		: Batch(_priority, _shaderIndex)
	{
		vertexArrayObjectID = 0;
		vertexBufferID = 0;
		indexBufferID = 0;

		cameraMatrixState = _cameraMatrixState;
		blending = _blending;
		textureDataID = _textureDataID;
		lineWidth = _lineWidth;
		drawMode = _drawMode;

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


	bool PrimitiveBatch::check(const Primitive &_primitive)
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


	bool PrimitiveBatch::render(const Camera2D* _batchCamera)
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
			shaderManager->getShader(shaderIndex)->uniforms->cameraMatrix = *_batchCamera->projectionMatrix;
		else
			shaderManager->getShader(shaderIndex)->uniforms->cameraMatrix = _batchCamera->staticMatrix;

		//Uniforms
		shaderManager->setUniforms(shaderIndex);

		//Draw
		glBindVertexArray(vertexArrayObjectID);
		if (lineWidth != 0.0f)
		{
			glLineWidth(lineWidth);
		}
		if (drawMode == LINE_TRIANGLE)
			glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_SHORT, (GLvoid*) NULL);
		else
			glDrawElements(drawMode, indices.size(), GL_UNSIGNED_SHORT, (GLvoid*) NULL);
		glBindVertexArray(0);

		checkOpenGLErrors(__FILE__, __LINE__);

		shaderManager->unuse(shaderIndex);

		console::drawCalls++;
		console::vertexDrawCount += vertices.size();

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
		glEnableVertexAttribArray(VertexAttributePosition::VERTEX_POSITION);
		glEnableVertexAttribArray(VertexAttributePosition::VERTEX_COLOR);
		glEnableVertexAttribArray(VertexAttributePosition::VERTEX_UV);
		
		glVertexAttribPointer(VertexAttributePosition::VERTEX_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(spehs::Vertex), reinterpret_cast<void*>(offsetof(spehs::Vertex, position)));
		/*FLOAT*///glVertexAttribPointer(VertexAttributePosition::VERTEX_COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(spehs::Vertex), reinterpret_cast<void*>(offsetof(spehs::Vertex, color)));
		/*UBYTE*/glVertexAttribPointer(VertexAttributePosition::VERTEX_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(spehs::Vertex), reinterpret_cast<void*>(offsetof(spehs::Vertex, color)));
		glVertexAttribPointer(VertexAttributePosition::VERTEX_UV, 2, GL_FLOAT, GL_FALSE, sizeof(spehs::Vertex), reinterpret_cast<void*>(offsetof(spehs::Vertex, uv)));

		//Unbind
		glBindVertexArray(0);

		glDisableVertexAttribArray(VertexAttributePosition::VERTEX_POSITION);
		glDisableVertexAttribArray(VertexAttributePosition::VERTEX_COLOR);
		glDisableVertexAttribArray(VertexAttributePosition::VERTEX_UV);
		
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
			//if (cameraMatrixState && getActiveBatchManager())
			//{
			//	//TODO: remove active batch manager dependecy from here?
			//	glPointSize(getActiveBatchManager()->getCamera2D()->scale * 2);
			//}
			//else
				glPointSize(1.0f);
		case LINE:
		case LINE_STRIP:
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

		case LINE_TRIANGLE:
			if (_numVertices > 2)
			{
				indices.resize(indices.size() + _numVertices * 2);
				for (unsigned i = 0; index < (numIndices + _numVertices * 2);)
				{
					indices[index++] = (GLushort) currentIndex;
					indices[index++] = (GLushort) currentIndex + ++i;
					indices[index++] = (GLushort) currentIndex + i;
					indices[index++] = (GLushort) currentIndex + ++i;
					indices[index++] = (GLushort) currentIndex + i++;
					indices[index++] = (GLushort) currentIndex;
				}
			}
			else
			{
				indices.resize(indices.size() + _numVertices);
				for (unsigned i = numIndices; i < numIndices + _numVertices; i++)
				{
					indices[i] = (GLushort) currentIndex + (i - numIndices);
				}
			}
			break;

		default:
			exceptions::fatalError(std::to_string(drawMode) + " - Draw mode is either undefined or is not supported by the engine at the moment!");
			break;
		}
	}
#pragma endregion


	//TEXT BATCH
#pragma region TEXT BATCH
	TextBatch::TextBatch(const bool _cameraMatrixState, const int16_t &_priority, const int &_shaderIndex) : Batch(_priority, _shaderIndex)
	{
		vertexArrayObjectID = 0;
		vertexBufferID = 0;
		indexBufferID = 0;

		cameraMatrixState = _cameraMatrixState;

		vertices.reserve(DEFAULT_MAX_BATCH_SIZE);
		indices.reserve((DEFAULT_MAX_BATCH_SIZE / 4) * 6);

		initBuffers();
	}
	TextBatch::~TextBatch()
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


	bool TextBatch::check(const Text &_text)
	{
		if (shaderIndex != _text.getShaderIndex() ||
			priority != _text.getPlaneDepth() ||
			cameraMatrixState != _text.getCameraMatrixState())
		{
			return false;
		}
		if (!isEnoughRoom(_text.worldVertexArray.size()))
		{
			return false;
		}
		return true;
	}


	bool TextBatch::render(const Camera2D* _batchCamera)
	{
		if (vertices.size() == 0)
			return false;

		setIndices();
		updateBuffers();

		//Blending
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);

		shaderManager->use(shaderIndex);

		//Camera Matrix
		if (cameraMatrixState)
			shaderManager->getShader(shaderIndex)->uniforms->cameraMatrix = *_batchCamera->projectionMatrix;
		else
			shaderManager->getShader(shaderIndex)->uniforms->cameraMatrix = _batchCamera->staticMatrix;

		//Uniforms
		shaderManager->setUniforms(shaderIndex);

		//Draw
		glBindVertexArray(vertexArrayObjectID);
		for (unsigned i = 0; i < textureIDs.size(); i++)
		{
			bind2DTexture(textureIDs[i], 0);
			glDrawElements(TRIANGLE, 6, GL_UNSIGNED_SHORT, reinterpret_cast<void*>((i * 6) * sizeof(GLushort)));
			console::drawCalls++;
		}
		glBindVertexArray(0);

		checkOpenGLErrors(__FILE__, __LINE__);

		shaderManager->unuse(shaderIndex);

		console::vertexDrawCount += vertices.size();

		//Clean up
		vertices.clear();
		indices.clear();
		textureIDs.clear();
		return true;
	}

	void TextBatch::push(Text* _text)
	{
		//VERTICES
		vertices.insert(vertices.end(), _text->worldVertexArray.begin(), _text->worldVertexArray.end());
		//TEXTURES
		textureIDs.insert(textureIDs.end(), _text->textureIDs.begin(), _text->textureIDs.end());
	}

	//Private:
	bool TextBatch::isEnoughRoom(const unsigned int &_numVertices)
	{
		if (_numVertices > DEFAULT_MAX_BATCH_SIZE)
			exceptions::fatalError("The number of vertices in the text exceeds the max amount allowed in the batch! Vertices: " + std::to_string(_numVertices));
		return (vertices.size() + _numVertices) <= DEFAULT_MAX_BATCH_SIZE;
	}

	void TextBatch::initBuffers()
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
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * (DEFAULT_MAX_BATCH_SIZE / 4) * 6, nullptr, GL_STREAM_DRAW);

		checkOpenGLErrors(__FILE__, __LINE__);

		//Attributes
		glEnableVertexAttribArray(VertexAttributePosition::VERTEX_POSITION);
		glEnableVertexAttribArray(VertexAttributePosition::VERTEX_COLOR);
		glEnableVertexAttribArray(VertexAttributePosition::VERTEX_UV);

		glVertexAttribPointer(VertexAttributePosition::VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(spehs::Vertex), reinterpret_cast<void*>(offsetof(spehs::Vertex, position)));
		/*FLOAT*///glVertexAttribPointer(VertexAttributePosition::VERTEX_COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(spehs::Vertex), reinterpret_cast<void*>(offsetof(spehs::Vertex, color)));
		/*UBYTE*/glVertexAttribPointer(VertexAttributePosition::VERTEX_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(spehs::Vertex), reinterpret_cast<void*>(offsetof(spehs::Vertex, color)));
		glVertexAttribPointer(VertexAttributePosition::VERTEX_UV, 2, GL_FLOAT, GL_FALSE, sizeof(spehs::Vertex), reinterpret_cast<void*>(offsetof(spehs::Vertex, uv)));

		//Unbind
		glBindVertexArray(0);

		glDisableVertexAttribArray(VertexAttributePosition::VERTEX_POSITION);
		glDisableVertexAttribArray(VertexAttributePosition::VERTEX_COLOR);
		glDisableVertexAttribArray(VertexAttributePosition::VERTEX_UV);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		checkOpenGLErrors(__FILE__, __LINE__);
	}

	void TextBatch::updateBuffers()
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

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

	void TextBatch::setIndices()
	{
		unsigned int currentIndex = 0;
		unsigned int index = 0;

		indices.resize((vertices.size() / 4) * 6);

		while (index < indices.size())
		{
			for (unsigned i = 1; i < 3;)
			{
				indices[index++] = ((GLushort) currentIndex);
				indices[index++] = ((GLushort) (currentIndex + i++));
				indices[index++] = ((GLushort) (currentIndex + i));
			}
			currentIndex += 4;
		}
	}
#pragma endregion
}