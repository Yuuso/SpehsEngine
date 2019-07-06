
#include "stdafx.h"

#include "SpehsEngine/Core/BitwiseOperations.h"

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
#include <glm/gtx/rotate_vector.hpp>

#include <algorithm>
#include <atomic>


static const size_t INITIAL_BUFFER_SIZE = 1024;
static const size_t ABSOLUTE_MAX_VERTICES = 65536;
static const size_t ABSOLUTE_MAX_INDICES = 65536;
static const size_t RECOMMENDED_MAX_VERTICES = 2048;
static const size_t RECOMMENDED_MAX_INDICES = 4096;
static const size_t RECOMMENDED_STATIC_MAX_VERTICES = ABSOLUTE_MAX_VERTICES;
static const size_t RECOMMENDED_STATIC_MAX_INDICES = ABSOLUTE_MAX_INDICES;


#ifdef _DEBUG
std::atomic<int> Batch3DAllocations;
std::atomic<int> Batch3DDeallocations;
#endif


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
			const Shader& shader = batchManager.shaderManager.getShader(shaderIndex);
			const Uniforms* const uniforms = shader.uniforms;
			se_assert(uniforms);
			se_assert(uniforms->textureData.size() >= textureDataIDs.size());
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

			const size_t recommendedMaxVertices((usage == GL_STATIC_DRAW) ? RECOMMENDED_STATIC_MAX_VERTICES : RECOMMENDED_MAX_VERTICES);
			const size_t recommendedMaxIndices((usage == GL_STATIC_DRAW) ? RECOMMENDED_STATIC_MAX_INDICES : RECOMMENDED_MAX_INDICES);
			const bool verticesOk = (vertices.size() + _numVertices) <= ((_numVertices > recommendedMaxVertices) ? ABSOLUTE_MAX_VERTICES : recommendedMaxVertices);
			const bool indicesOk = (indices.size() + _numIndices) <= ((_numVertices > recommendedMaxIndices) ? ABSOLUTE_MAX_INDICES : recommendedMaxIndices);
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

			Shader& shader = batchManager.shaderManager.getShader(shaderIndex);
			shader.shader.use();

			// Texture
			Uniforms* uniforms = shader.uniforms;
			se_assert(uniforms);
			se_assert(uniforms->textureData.size() >= textureDataIDs.size());
			for (size_t i = 0; i < textureDataIDs.size(); i++)
				uniforms->textureData[i].textureDataID = textureDataIDs[i];

			uniforms->cameraMatrix = *batchManager.camera3D.cameraMatrix;

			// Uniforms
			uniforms->setUniforms();

			// Draw
			glBindVertexArray(vertexArrayObjectID);
			glDrawElements(drawMode, GLsizei(indices.size()), GL_UNSIGNED_SHORT, reinterpret_cast<void*>(0));
			glBindVertexArray(0);

			checkOpenGLErrors(__FILE__, __LINE__);

			shader.shader.unuse();

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
			needVertexBufferUpdate = true;
			needIndexBufferUpdate = true;
			return objectIndices;
		}

		void MeshBatch::remove(const std::pair<size_t, size_t> _index, std::pair<size_t, size_t> _size)
		{
			vertices.erase(vertices.begin() + _index.first, vertices.begin() + _index.first + _size.first);
			indices.erase(indices.begin() + _index.second, indices.begin() + _index.second + _size.second);

			for (size_t i = _index.second; i < indices.size(); i++)
				indices[i] -= (GLushort)_size.first;
			needVertexBufferUpdate = true;
			needIndexBufferUpdate = true;
		}

		void MeshBatch::updateVertices(const size_t _index, const Mesh& _mesh, const int _updates)
		{
#if false
			if (usage == GL_STATIC_DRAW)
				se::log::info("Updating static batch with " + std::to_string(vertices.size()) + " vertices, update: " + std::to_string(_updates));
#endif
			// T = t * R * S
			glm::vec4 newVertex;
			glm::mat4 transformMatrix;

			const size_t size = _mesh.vertexArray.size();
			if (checkBit(_updates, VERTEX_UPDATE_VERTEX) || _mesh.billboarding != Billboarding::disabled)
			{
				transformMatrix = glm::translate(_mesh.localPosition + _mesh.meshPosition) * glm::mat4_cast(_mesh.localRotation * _mesh.meshRotation) * glm::scale(_mesh.localScale * _mesh.meshScale);
				transformMatrix = glm::translate(_mesh.position) * glm::mat4_cast(_mesh.rotation) * glm::scale(_mesh.scale) * transformMatrix;
				for (size_t i = 0; i < size; i++)
				{
					// Vertices
					newVertex = transformMatrix * glm::vec4(_mesh.vertexArray[i].position, 1.0f);
					vertices[_index + i].position.x = newVertex.x;
					vertices[_index + i].position.y = newVertex.y;
					vertices[_index + i].position.z = newVertex.z;
				}
			}
			if (_mesh.billboarding != Billboarding::disabled)
			{
				const glm::vec3& cameraUp = batchManager.camera3D.getWorldUp();
				const glm::vec3& cameraRight = batchManager.camera3D.getWorldRight();

				se_assert_m(size == 4, "Billboarding is only supported for planes!");
				const float width = _mesh.scale.x;
				const float height = _mesh.scale.z;

				glm::vec3 centerPosition(0.0f);
				for (size_t i = 0; i < size; i++)
					centerPosition += vertices[_index + i].position;
				centerPosition /= (float)size;
				// Todo: Take position into account to here -> possible to make pivot point somewhere other than center

				glm::vec3 newPosition;
				for (size_t i = 0; i < size; i++)
				{
					newPosition = centerPosition;
					const glm::vec2 vertexPosition = glm::rotate(glm::vec2(_mesh.vertexArray[i].position.x, _mesh.vertexArray[i].position.z), _mesh.rotation.x);
					if (_mesh.billboarding == Billboarding::x_only || _mesh.billboarding == Billboarding::enabled)
					{
						newPosition += cameraRight * width * vertexPosition.x;
					}
					else
					{
						// Don't know what should be here...
						se_assert_m(false, "Billboarding::z_only not properly implemented.");
						newPosition.x -= centerPosition.x;
						newPosition.x += vertices[_index + i].position.x;
					}
					if (_mesh.billboarding == Billboarding::z_only || _mesh.billboarding == Billboarding::enabled)
					{
						newPosition += cameraUp * height * -vertexPosition.y; // negative? Is this right?
					}
					else
					{
						// Don't know what should be here...
						se_assert_m(false, "Billboarding::x_only not properly implemented.");
						newPosition.z -= centerPosition.z;
						newPosition.z += vertices[_index + i].position.z;
					}

					vertices[_index + i].position = newPosition;
				}
			}
			if (checkBit(_updates, VERTEX_UPDATE_NORMAL))
			{
				const glm::mat4 normalMatrix = glm::mat4(glm::inverse(glm::transpose(glm::mat3(transformMatrix))));
				for (size_t i = 0; i < size; i++)
				{
					// Normals
					newVertex = normalMatrix * glm::vec4(_mesh.vertexArray[i].normal, 1.0f);
					newVertex = glm::normalize(newVertex);
					vertices[_index + i].normal.x = newVertex.x;
					vertices[_index + i].normal.y = newVertex.y;
					vertices[_index + i].normal.z = newVertex.z;
				}
			}
			if (checkBit(_updates, VERTEX_UPDATE_COLOR))
			{
				for (size_t i = 0; i < size; i++)
				{
					// Color
					vertices[_index + i].color = _mesh.vertexArray[i].color;
				}
			}
			needVertexSubBufferUpdate = true;
			bufferUpdateVertexOffset = glm::min(bufferUpdateVertexOffset, _index);
			bufferUpdateVertexEnd = glm::max(bufferUpdateVertexEnd, _index + size);
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
			glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex3D) * INITIAL_BUFFER_SIZE, nullptr, usage);
			bufferUpdateVertexOffset = INITIAL_BUFFER_SIZE;
			bufferUpdateVertexEnd = 0;

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * INITIAL_BUFFER_SIZE, nullptr, usage);

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
			if (!needVertexBufferUpdate && !needIndexBufferUpdate && !needVertexSubBufferUpdate)
				return;
			glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

			if (needVertexBufferUpdate)
			{
				glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex3D) * vertices.size(), &vertices[0], usage);
				needVertexBufferUpdate = false;
				needVertexSubBufferUpdate = false;
			}
			else if (needVertexSubBufferUpdate)
			{
				glBufferSubData(GL_ARRAY_BUFFER,
								sizeof(Vertex3D) * bufferUpdateVertexOffset,
								sizeof(Vertex3D) * (bufferUpdateVertexEnd - bufferUpdateVertexOffset),
								&vertices[bufferUpdateVertexOffset]);
				needVertexSubBufferUpdate = false;
			}
			bufferUpdateVertexOffset = vertices.size();
			bufferUpdateVertexEnd = 0;

			if (needIndexBufferUpdate)
			{
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * indices.size(), &indices[0], usage);
				needIndexBufferUpdate = false;
			}

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			checkOpenGLErrors(__FILE__, __LINE__);
		}
	}
}