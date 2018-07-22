
#pragma once

#include "SpehsEngine/Rendering/Vertex3D.h"
#include "SpehsEngine/Core/Color.h"

#include <glm/vec3.hpp>
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
		class MeshBatch;
		class ModelManager;
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
			void setPosition(const glm::vec3& _newPosition);
			void setRotation(const glm::vec3& _newRotation);
			void setScale(const glm::vec3& _newScale);
			void setColor(const Color _color);
			void setAlpha(const float _alpha);
			void translate(const glm::vec3& _translation);

			void setBackFaceCulling(const bool _value);
			void setRenderState(const bool _newState);
			void toggleRenderState();
			void setBlending(const bool _value);
			void setDepthTest(const bool _value);
			void setShaderIndex(const unsigned int _newShaderIndex);

			TextureData* setTexture(const std::string& _texturePath);
			TextureData* setTexture(const size_t _textureID);
			void setTexture(TextureData* _textureDataPtr);

			glm::vec3 getPosition() const { return position; }
			glm::vec3 getRotation() const { return rotation; }
			glm::vec3 getScale() const { return scale; }
			se::Color getColor() const { return color; }
			float getAlpha() const { return color.a; }

			bool getBackFaceCulling() const { return backFaceCulling; }
			bool getRenderState() const { return renderState; }
			bool getBlending() const { return blending; }
			bool getDepthTest() const { return depthTest; }
			unsigned int getShaderIndex() const { return shaderIndex; }

		protected:
			Mesh();
			~Mesh();

			BatchManager3D* batchManager = nullptr;

			bool needUpdate = true;

			bool backFaceCulling = true;
			bool renderState = true; // Whether Mesh is rendered or not
			bool blending = false;
			bool depthTest = true;
			unsigned int shaderIndex;
			GLuint textureDataID = 0;
			GLenum drawMode;
			glm::vec3 position = glm::vec3(0.0f);
			glm::vec3 rotation = glm::vec3(0.0f);
			glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
			Color color = Color(255, 255, 255, 255);

			std::vector<Vertex3D> vertexArray;
			std::vector<GLushort> elementArray;

		private:
			Mesh(const Mesh& _other) = delete;
			Mesh(const Mesh&& _other) = delete;
			Mesh& operator=(const Mesh& _other) = delete;
			Mesh& operator=(const Mesh&& _other) = delete;
		};
	}
}
