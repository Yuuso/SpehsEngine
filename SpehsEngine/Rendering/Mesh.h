
#pragma once

#include "SpehsEngine/Rendering/Vertex3D.h"
#include "SpehsEngine/Core/Color.h"

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>

#include <string>
#include <vector>


typedef unsigned short GLushort;
typedef unsigned int GLuint;
typedef unsigned int GLenum;


namespace se
{
	namespace rendering
	{
		class TextureManager;
		struct TextureData;

		class Mesh
		{
			friend class BatchManager3D;
			friend class MeshBatch;
			friend class ModelManager;
			friend class Model;
			friend class Plane3D;
			friend class Line3D;

		public:
			Mesh();
			~Mesh();
			void setName(const std::string& _name);

			void setVertices(const std::vector<Vertex3D>& _vertices);
			void setIndices(const std::vector<GLushort>& _indices);

			void setLocalPosition(const glm::vec3& _newPosition);
			void setLocalRotation(const glm::quat& _newRotation);
			void setLocalScale(const glm::vec3& _newScale);
			void translateLocal(const glm::vec3& _translation);
			void setColor(const Color _color);
			void setAlpha(const float _alpha);
			void translate(const glm::vec3& _translation);

			void setBackFaceCulling(const bool _value);
			void setRenderState(const bool _newState);
			void toggleRenderState();
			void setBlending(const bool _value);
			void setDepthTest(const bool _value);
			void setShaderIndex(const unsigned int _newShaderIndex);

			void setTexture(TextureManager& _textureManager, const std::string& _texturePath, const size_t _index = 0);
			void setTexture(TextureManager& _textureManager, const size_t _textureID, const size_t _index = 0);
			void setTexture(TextureData* _textureDataPtr, const size_t _index);

			const std::string& getName() const { return name; }
			const std::vector<Vertex3D>& getVertices() const { return vertexArray; }
			const std::vector<GLushort>& getIndices() const { return elementArray; }

			glm::vec3 getLocalPosition() const { return localPosition; }
			glm::quat getLocalRotation() const { return localRotation; }
			glm::vec3 getLocalScale() const { return localScale; }
			glm::vec3 getPosition() const { return position + localPosition; }
			glm::quat getRotation() const { return rotation * localRotation; }
			glm::vec3 getScale() const { return scale * localScale; }

			se::Color getColor() const { return color; }
			float getAlpha() const { return color.a; }

			bool getBackFaceCulling() const { return backFaceCulling; }
			bool getRenderState() const { return renderState; }
			bool getBlending() const { return blending; }
			bool getDepthTest() const { return depthTest; }
			unsigned int getShaderIndex() const { return shaderIndex; }

		protected:
			Mesh(const Mesh& _other);

			std::vector<BatchManager3D*> batchManagers;

			bool needUpdate = true;
			void unbatch();

			bool backFaceCulling = true;
			bool renderState = true; // Whether Mesh is rendered or not
			bool blending = false;
			bool depthTest = true;
			unsigned int shaderIndex;
			std::vector<GLuint> textureDataIDs;
			GLenum drawMode;
			Color color;

			void updatePosition(const glm::vec3& _newPosition);
			void updateRotation(const glm::quat& _newRotation);
			void updateScale(const glm::vec3& _newScale);

			std::string name;
			std::vector<Vertex3D> vertexArray;
			std::vector<GLushort> elementArray;

		private:
			Mesh(const Mesh&& _other) = delete;
			Mesh& operator=(const Mesh& _other) = delete;
			Mesh& operator=(const Mesh&& _other) = delete;

			glm::vec3 localPosition;
			glm::quat localRotation;
			glm::vec3 localScale;
			glm::vec3 position;
			glm::quat rotation;
			glm::vec3 scale;
		};
	}
}
