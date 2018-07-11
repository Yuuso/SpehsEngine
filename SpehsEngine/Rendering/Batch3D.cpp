
#include "SpehsEngine/Rendering/Batch3D.h"
#include "SpehsEngine/Rendering/BatchManager3D.h"
#include "SpehsEngine/Rendering/Mesh.h"
#include "SpehsEngine/Rendering/ShaderManager.h"
#include "SpehsEngine/Rendering/OpenGLError.h"
#include "SpehsEngine/Rendering/Camera3D.h"

#include <GL/glew.h>
#include <algorithm>
#include <atomic>


std::atomic<int> Batch3DAllocations;
std::atomic<int> Batch3DDeallocations;


namespace spehs
{
	static inline int getIndexMultiplier(const size_t _batchSize)
	{
		return int((_batchSize - 2) * 3);
	}

	MeshBatch::MeshBatch(BatchManager3D& _batchManager, const int _shaderIndex, const GLuint _textureDataID, const bool _depthTest, const bool _blending, const bool _backFaceCulling)
		: batchManager(_batchManager)
		, shaderIndex(_shaderIndex)
		, textureDataID(_textureDataID)
		, depthTest(_depthTest)
		, blending(_blending)
		, backFaceCulling(_backFaceCulling)
	{
#ifdef _DEBUG
		Batch3DAllocations++;
#endif
		vertexArrayObjectID = 0;
		vertexBufferID = 0;
		indexBufferID = 0;

		vertices.reserve(DEFAULT_MAX_BATCH_SIZE);
		indices.reserve(getIndexMultiplier(DEFAULT_MAX_BATCH_SIZE));

		batchSize = DEFAULT_MAX_BATCH_SIZE;
		indexSize = getIndexMultiplier(DEFAULT_MAX_BATCH_SIZE);

		initBuffers();
	}

	MeshBatch::~MeshBatch()
	{
#ifdef _DEBUG
		Batch3DDeallocations++;
#endif
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

	bool MeshBatch::check(const Mesh& _mesh)
	{
		if (shaderIndex != _mesh.shaderIndex ||
			textureDataID != _mesh.textureDataID ||
			blending != _mesh.blending ||
			depthTest != _mesh.depthTest)
		{
			return false;
		}
		if (!isEnoughRoom(_mesh.worldVertexArray.size()))
		{
			return false;
		}
		return true;
	}

	bool MeshBatch::render(BatchRenderResults* results)
	{
		if (vertices.size() == 0)
			return false;

		if (depthTest)
		{
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
		}

		if (blending)
		{
			glEnable(GL_BLEND);
			if (depthTest)
			{
				glBlendEquation(GL_FUNC_ADD);
				glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
				//glBlendFunc(GL_ONE, GL_ONE);
				glDepthMask(GL_FALSE); // no depth masking, read only
			}
			else
			{
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}
		}
		else
		{
			glDisable(GL_BLEND);
		}

		if (backFaceCulling)
		{
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
		}
		else
		{
			glDisable(GL_CULL_FACE);
		}

		updateBuffers();

		batchManager.shaderManager.use(shaderIndex);

		//Texture
		if (textureDataID)
		{
			batchManager.shaderManager.getShader(shaderIndex).uniforms->textureDataID = textureDataID;
		}

		batchManager.shaderManager.getShader(shaderIndex).uniforms->cameraMatrix = *batchManager.camera3D.cameraMatrix;

		//Uniforms
		batchManager.shaderManager.setUniforms(shaderIndex);

		//Draw
		glBindVertexArray(vertexArrayObjectID);
		glDrawElements(GL_TRIANGLES, GLsizei(indices.size()), GL_UNSIGNED_SHORT, reinterpret_cast<void*>(0));
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

	void MeshBatch::push(Mesh* _mesh)
	{
		GLushort firstElement = (GLushort)vertices.size();
		size_t indSize = indices.size();
		//INDICES
		indices.insert(indices.end(), _mesh->elementArray.begin(), _mesh->elementArray.end());
		if (firstElement != 0)
		{
			for (size_t i = indSize; i < indices.size(); i++)
			{
				indices[i] += firstElement;
			}
		}
		//VERTICES
		vertices.insert(vertices.end(), _mesh->worldVertexArray.begin(), _mesh->worldVertexArray.end());
	}

	bool MeshBatch::isEnoughRoom(const size_t _numVertices)
	{
		if (_numVertices > DEFAULT_MAX_BATCH_SIZE)
		{
			if (vertices.size() == 0) //If this is empty batch, resize it for the mesh
			{
				batchSize = _numVertices;
				glBindVertexArray(vertexArrayObjectID);
				glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
				glBufferData(GL_ARRAY_BUFFER, sizeof(spehs::Vertex3D) * batchSize, nullptr, GL_STREAM_DRAW);
				glBindVertexArray(0);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				return true;
			}
			return false;
		}
		return (vertices.size() + _numVertices) <= DEFAULT_MAX_BATCH_SIZE;
	}
	void MeshBatch::initBuffers()
	{
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
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * indexSize, nullptr, GL_STREAM_DRAW);

		checkOpenGLErrors(__FILE__, __LINE__);

		//Attributes
		batchManager.shaderManager.use(shaderIndex);

		batchManager.shaderManager.set3D(shaderIndex);

		//Unbind
		glBindVertexArray(0);

		batchManager.shaderManager.unuse(shaderIndex);

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
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(spehs::Vertex3D) * vertices.size(), &vertices[0]);

		//Check indices
		if (indices.size() > indexSize)
		{
			indexSize = indices.size();
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * indexSize, nullptr, GL_STREAM_DRAW);
		}
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(GLushort) * indices.size(), &indices[0]);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		checkOpenGLErrors(__FILE__, __LINE__);
	}
}