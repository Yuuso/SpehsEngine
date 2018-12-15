
#include "stdafx.h"

#include "SpehsEngine/Rendering/Batch3D.h"
#include "SpehsEngine/Rendering/BatchManager3D.h"
#include "SpehsEngine/Rendering/Mesh.h"
#include "SpehsEngine/Rendering/ShaderManager.h"
#include "SpehsEngine/Rendering/OpenGLError.h"
#include "SpehsEngine/Rendering/Camera3D.h"
#include "SpehsEngine/Rendering/BatchRenderResults.h"

#include <GL/glew.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <algorithm>
#include <atomic>

// TODO: Some sense to these values? At least experiment what works best...

static const size_t INITIAL_BUFFER_SIZE = 2048;
static const size_t RECOMMENDED_MAX_VERTICES = 4096;
static const size_t RECOMMENDED_MAX_INDICES = 4096;
static const size_t ABSOLUTE_MAX_VERTICES = 65536;
static const size_t ABSOLUTE_MAX_INDICES = 65536;


std::atomic<int> Batch3DAllocations;
std::atomic<int> Batch3DDeallocations;


namespace se
{
	namespace rendering
	{
		MeshBatch::MeshBatch(BatchManager3D& _batchManager, const int _shaderIndex, const std::vector<GLuint> _textureDataIDs,
								const bool _depthTest, const bool _blending, const bool _backFaceCulling, const GLenum _drawMode,
								const float _lineWidth, const bool _lineSmoothing, const bool _staticDraw)
			: batchManager(_batchManager)
			, backFaceCulling(_backFaceCulling)
			, blending(_blending)
			, depthTest(_depthTest)
			, shaderIndex(_shaderIndex)
			, textureDataIDs(_textureDataIDs)
			, drawMode(_drawMode)
			, lineWidth(_lineWidth)
			, lineSmoothing(_lineSmoothing)
			, usage(_staticDraw ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW)
		{
#ifdef _DEBUG
			Batch3DAllocations++;
#endif
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

		bool MeshBatch::checkCompatibility(const Mesh& _mesh) const
		{
			if (backFaceCulling != _mesh.backFaceCulling ||
				blending != _mesh.blending ||
				depthTest != _mesh.depthTest ||
				shaderIndex != _mesh.shaderIndex ||
				textureDataIDs.size() != _mesh.textureDataIDs.size() ||
				drawMode != _mesh.drawMode ||
				lineWidth != _mesh.lineWidth ||
				lineSmoothing != _mesh.smoothLine ||
				(_mesh.staticDraw ? (usage != GL_STATIC_DRAW) : (usage != GL_DYNAMIC_DRAW)))
			{
				return false;
			}
			for (size_t i = 0; i < textureDataIDs.size(); i++)
			{
				if (textureDataIDs[i] != _mesh.textureDataIDs[i])
					return false;
			}
			return checkSize(_mesh.vertexArray.size(), _mesh.elementArray.size());
		}

		bool MeshBatch::checkSize(const size_t _numVertices, const size_t _numIndices) const
		{
			if (_numVertices >= ABSOLUTE_MAX_VERTICES || _numIndices >= ABSOLUTE_MAX_INDICES)
				log::error("Max batch size exceeded!");
						
			const bool verticesOk = (vertices.size() + _numVertices) <= ((_numVertices > RECOMMENDED_MAX_VERTICES) ? ABSOLUTE_MAX_VERTICES : RECOMMENDED_MAX_VERTICES);
			const bool indicesOk = (indices.size() + _numIndices) <= ((_numVertices > RECOMMENDED_MAX_INDICES) ? ABSOLUTE_MAX_INDICES : RECOMMENDED_MAX_INDICES);
			return (verticesOk && indicesOk);
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

			// Lines
			{
				glLineWidth(lineWidth);
				if (lineSmoothing)
				{
					glEnable(GL_LINE_SMOOTH);
				}
				else
				{
					glDisable(GL_LINE_SMOOTH);
				}
			}

			updateBuffers();

			batchManager.shaderManager.use(shaderIndex);

			//Texture
			for (size_t i = 0; i < textureDataIDs.size(); i++)
				batchManager.shaderManager.getShader(shaderIndex).uniforms->textureData[i].textureDataID = textureDataIDs[i];

			batchManager.shaderManager.getShader(shaderIndex).uniforms->cameraMatrix = *batchManager.camera3D.cameraMatrix;

			//Uniforms
			batchManager.shaderManager.setUniforms(shaderIndex);

			//Draw
			glBindVertexArray(vertexArrayObjectID);
			glDrawElements(drawMode, GLsizei(indices.size()), GL_UNSIGNED_SHORT, reinterpret_cast<void*>(0));
			glBindVertexArray(0);

			checkOpenGLErrors(__FILE__, __LINE__);

			batchManager.shaderManager.unuse(shaderIndex);

			if (results)
			{
				results->drawCalls++;
				results->vertexCount += vertices.size();
			}
			return true;
		}

		std::pair<size_t, size_t> MeshBatch::push(const Mesh& _mesh)
		{
			if (_mesh.vertexArray.size() >= ABSOLUTE_MAX_VERTICES || _mesh.elementArray.size() >= ABSOLUTE_MAX_INDICES)
				log::error("Max batch size exceeded!");

			const std::pair<size_t, size_t> objectIndices = std::make_pair(vertices.size(), indices.size());
			GLushort firstElement = (GLushort)vertices.size();
			size_t sizeIndices = indices.size();

			indices.insert(indices.end(), _mesh.elementArray.begin(), _mesh.elementArray.end());
			if (firstElement != 0)
			{
				for (size_t i = sizeIndices; i < indices.size(); i++)
				{
					indices[i] += firstElement;
				}
			}

			vertices.insert(vertices.end(), _mesh.vertexArray.begin(), _mesh.vertexArray.end());
			needBufferUpdate = true;
			return objectIndices;
		}

		void MeshBatch::remove(const std::pair<size_t, size_t> _index, std::pair<size_t, size_t> _size)
		{
			vertices.erase(vertices.begin() + _index.first, vertices.begin() + _index.first + _size.first);
			indices.erase(indices.begin() + _index.second, indices.begin() + _index.second + _size.second);

			for (size_t i = _index.second; i < indices.size(); i++)
				indices[i] -= (GLushort)_size.first;
			needBufferUpdate = true;
		}

		void MeshBatch::updateVertices(const size_t _index, const Mesh& _mesh)
		{
			// T = t * R * S
			glm::mat4 scaledMatrix = glm::scale(_mesh.getScale());
			glm::mat4 scaledRotatedMatrix = glm::mat4_cast(_mesh.getRotation()) * scaledMatrix;
			glm::mat4 transformMatrix = glm::translate(_mesh.getPosition()) * scaledRotatedMatrix;
			glm::mat4 normalMatrix = glm::mat4(glm::inverse(glm::transpose(glm::mat3(transformMatrix))));

			glm::vec4 newVertex;
			Vertex3D* oldVertex;
			const size_t size = _mesh.vertexArray.size();
			for (size_t i = 0; i < size; i++)
			{
				oldVertex = &vertices[_index + i];
				// Vertices
				newVertex = transformMatrix * glm::vec4(_mesh.vertexArray[i].position, 1.0f);
				oldVertex->position.x = newVertex.x;
				oldVertex->position.y = newVertex.y;
				oldVertex->position.z = newVertex.z;
				// Color
				oldVertex->color = _mesh.vertexArray[i].color;
				// Normals
				newVertex = normalMatrix * glm::vec4(_mesh.vertexArray[i].normal, 1.0f);
				newVertex = glm::normalize(newVertex);
				oldVertex->normal.x = newVertex.x;
				oldVertex->normal.y = newVertex.y;
				oldVertex->normal.z = newVertex.z;
			}
			needBufferUpdate = true;
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
			vertexBufferSize = INITIAL_BUFFER_SIZE;
			glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex3D) * vertexBufferSize, nullptr, usage);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
			indexBufferSize = INITIAL_BUFFER_SIZE;
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * indexBufferSize, nullptr, usage);

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
			if (!needBufferUpdate)
				return;
			glBindVertexArray(vertexArrayObjectID);
			glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

			size_t size = vertices.size();
			if (size > vertexBufferSize)
			{
				vertexBufferSize = size;
				glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex3D) * size, &vertices[0], usage);
			}
			else
			{
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex3D) * size, &vertices[0]);
			}

			size = indices.size();
			if (size > indexBufferSize)
			{
				indexBufferSize = size;
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * size, &indices[0], usage);
			}
			else
			{
				glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(GLushort) * size, &indices[0]);
			}

			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			checkOpenGLErrors(__FILE__, __LINE__);
			needBufferUpdate = false;
		}
	}
}