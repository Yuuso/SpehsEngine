
#include "SpehsEngine/Rendering/Batch.h"
#include "SpehsEngine/Rendering/Primitive.h"
#include "SpehsEngine/Rendering/BatchManager.h"
#include "SpehsEngine/Rendering/ShaderManager.h"
#include "SpehsEngine/Rendering/OpenGLError.h"
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
	Batch::Batch(BatchManager& _batchManager, const PlaneDepth _priority, const size_t _shaderIndex, const bool _cameraMatrixState)
		: batchManager(_batchManager)
		, priority(_priority)
		, shaderIndex(_shaderIndex)
		, cameraMatrixState(_cameraMatrixState)
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


	int getIndexMultiplier(const DrawMode _drawMode, const size_t _batchSize)
	{
		switch (_drawMode)
		{
		case DrawMode::TRIANGLE:
			return int((_batchSize - 2) * 3);
			break;
		default:
			return int(_batchSize);
			break;
		}
	}

	//PRIMITIVE BATCH
#pragma region PRIMITIVE BATCH
	PrimitiveBatch::PrimitiveBatch(BatchManager& _batchManager, const bool _cameraMatrixState, const PlaneDepth _priority, const bool _blending,
									const int _shaderIndex, const GLuint _textureDataID, const DrawMode _drawMode, const float _lineWidth)
		: Batch(_batchManager, _priority, _shaderIndex, _cameraMatrixState)
		, blending(_blending)
		, textureDataID(_textureDataID)
		, drawMode(_drawMode)
		, lineWidth(_lineWidth)
	{
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
		if (drawMode == DrawMode::LINE ||
			drawMode == DrawMode::LINE_LOOP ||
			drawMode == DrawMode::LINE_STRIP ||
			drawMode == DrawMode::LINE_STRIP_ADJACENCY ||
			drawMode == DrawMode::LINE_ADJACENCY)
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


	bool PrimitiveBatch::render(BatchRenderResults* results)
	{
		if (vertices.size() == 0)
			return false;

		updateBuffers();

		//Blending
		if (blending)
		{
			glEnable(GL_BLEND);
			glDisable(GL_DEPTH_TEST);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		else
		{
			glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
		}

		batchManager.shaderManager.use(shaderIndex);

		//Texture
		if (textureDataID)
		{
			batchManager.shaderManager.getShader(shaderIndex).uniforms->textureDataID = textureDataID;
			if (drawMode == DrawMode::POINT)
			{
				glEnable(GL_POINT_SPRITE);
				glEnable(GL_PROGRAM_POINT_SIZE);
			}
		}

		//Camera Matrix
		if (cameraMatrixState)
			batchManager.shaderManager.getShader(shaderIndex).uniforms->cameraMatrix = *batchManager.camera2D.projectionMatrix;
		else
			batchManager.shaderManager.getShader(shaderIndex).uniforms->cameraMatrix = batchManager.camera2D.staticMatrix;

		//Uniforms
		batchManager.shaderManager.setUniforms(shaderIndex);

		//Draw
		glBindVertexArray(vertexArrayObjectID);
		if (lineWidth != 0.0f)
		{
			glLineWidth(lineWidth);
		}
		if (drawMode == DrawMode::LINE_TRIANGLE)
			glDrawElements(GL_LINES, GLsizei(indices.size()), GL_UNSIGNED_SHORT, (GLvoid*) NULL);
		else
			glDrawElements((GLenum)drawMode, GLsizei(indices.size()), GL_UNSIGNED_SHORT, (GLvoid*) NULL);
		glBindVertexArray(0);

		checkOpenGLErrors(__FILE__, __LINE__);

		batchManager.shaderManager.unuse(shaderIndex);

		if (results)
		{
			results->drawCalls++;
			results->vertexCount += vertices.size();
		}

		//Clean up
		vertices.clear();
		indices.clear();
		return true;
	}

	void PrimitiveBatch::push(const Primitive& _primitive)
	{
		//INDICES
		setIndices(_primitive.worldVertexArray.size());
		//VERTICES
		vertices.insert(vertices.end(), _primitive.worldVertexArray.begin(), _primitive.worldVertexArray.end());
	}

	//Private:
	bool PrimitiveBatch::isEnoughRoom(const size_t _numVertices)
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
		batchManager.shaderManager.use(shaderIndex);

		batchManager.shaderManager.set2D(shaderIndex);

		//Unbind
		glBindVertexArray(0);

		batchManager.shaderManager.unuse(shaderIndex);

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

	void PrimitiveBatch::setIndices(const size_t _numVertices)
	{
		size_t currentIndex = (vertices.size());
		size_t numIndices = (indices.size());
		size_t index = (numIndices);
		switch (drawMode)
		{
		case DrawMode::UNDEFINED:
			exceptions::fatalError(std::to_string((unsigned int)drawMode) + " - Batch's DrawMode is UNDEFINED!");
			break;

		case DrawMode::POINT:
			//if (cameraMatrixState && getActiveBatchManager())
			//{
			//	//TODO: remove active batch manager dependecy from here?
			//	glPointSize(getActiveBatchManager()->getCamera2D()->scale * 2);
			//}
			//else
				glPointSize(1.0f);
		case DrawMode::LINE:
		case DrawMode::LINE_STRIP:
		case DrawMode::LINE_LOOP:
			indices.resize(indices.size() + _numVertices);
			for (size_t i = numIndices; i < numIndices + _numVertices; i++)
			{
				indices[i] = (GLushort) (currentIndex + (i - numIndices));
			}
			break;

		case DrawMode::TRIANGLE:
			/*
			Number of indices in a polygon:
			(NumberOfVertices - 2) * 3
			*/
			indices.resize(indices.size() + ((_numVertices - 2) * 3));
			for (size_t i = 1; index < (numIndices + ((_numVertices - 2) * 3));)
			{
				indices[index++] = ((GLushort) currentIndex);
				indices[index++] = ((GLushort) (currentIndex + i++));
				indices[index++] = ((GLushort) (currentIndex + i));
			}
			break;

		case DrawMode::LINE_TRIANGLE:
			if (_numVertices > 2)
			{
				indices.resize(indices.size() + _numVertices * 2);
				for (size_t i = 0; index < (numIndices + _numVertices * 2);)
				{
					indices[index++] = (GLushort) currentIndex;
					indices[index++] = (GLushort) (currentIndex + ++i);
					indices[index++] = (GLushort) (currentIndex + i);
					indices[index++] = (GLushort) (currentIndex + ++i);
					indices[index++] = (GLushort) (currentIndex + i++);
					indices[index++] = (GLushort) currentIndex;
				}
			}
			else
			{
				indices.resize(indices.size() + _numVertices);
				for (size_t i = numIndices; i < numIndices + _numVertices; i++)
				{
					indices[i] = (GLushort) (currentIndex + (i - numIndices));
				}
			}
			break;

		default:
			exceptions::fatalError(std::to_string((unsigned int)drawMode) + " - Draw mode is either undefined or is not supported by the engine at the moment!");
			break;
		}
	}
#pragma endregion


	//TEXT BATCH
#pragma region TEXT BATCH
	TextBatch::TextBatch(BatchManager& _batchManager, const bool _cameraMatrixState, const PlaneDepth _priority, const size_t _shaderIndex)
		: Batch(_batchManager, _priority, _shaderIndex, _cameraMatrixState)
	{
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

	bool TextBatch::check(const Text& _text)
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

	bool TextBatch::render(BatchRenderResults* results)
	{
		if (vertices.empty())
			return false;

		setIndices();
		updateBuffers();

		//Blending
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		batchManager.shaderManager.use(shaderIndex);

		//Camera Matrix
		if (cameraMatrixState)
			batchManager.shaderManager.getShader(shaderIndex).uniforms->cameraMatrix = *batchManager.camera2D.projectionMatrix;
		else
			batchManager.shaderManager.getShader(shaderIndex).uniforms->cameraMatrix = batchManager.camera2D.staticMatrix;

		//Uniforms
		batchManager.shaderManager.setUniforms(shaderIndex);

		//Draw
		glBindVertexArray(vertexArrayObjectID);
		for (size_t i = 0; i < textureIDs.size(); i++)
		{
			bind2DTexture(textureIDs[i], 0);
			glDrawElements((GLenum)DrawMode::TRIANGLE, 6u, GL_UNSIGNED_SHORT, reinterpret_cast<void*>((i * 6) * sizeof(GLushort)));
		}
		glBindVertexArray(0);

		checkOpenGLErrors(__FILE__, __LINE__);

		batchManager.shaderManager.unuse(shaderIndex);

		if (results)
		{
			results->drawCalls += textureIDs.size();
			results->vertexCount += vertices.size();
		}

		//Clean up
		vertices.clear();
		indices.clear();
		textureIDs.clear();
		return true;
	}

	void TextBatch::push(const Text& _text)
	{
		//VERTICES
		vertices.insert(vertices.end(), _text.worldVertexArray.begin(), _text.worldVertexArray.end());
		//TEXTURES
		textureIDs.insert(textureIDs.end(), _text.textureIDs.begin(), _text.textureIDs.end());
	}

	//Private:
	bool TextBatch::isEnoughRoom(const size_t _numVertices)
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
		batchManager.shaderManager.use(shaderIndex);

		batchManager.shaderManager.set2D(shaderIndex);

		//Unbind
		glBindVertexArray(0);

		batchManager.shaderManager.unuse(shaderIndex);

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
		size_t currentIndex = 0;
		size_t index = 0;

		indices.resize((vertices.size() / 4) * 6);

		while (index < indices.size())
		{
			for (size_t i = 1; i < 3;)
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