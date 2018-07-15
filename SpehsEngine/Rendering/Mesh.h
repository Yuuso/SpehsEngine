
#pragma once

#include "SpehsEngine/Rendering/Vertex3D.h"
#include "SpehsEngine/Core/Color.h"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include <string>
#include <vector>


typedef unsigned short GLushort;
typedef unsigned int GLuint;


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

		public:
			Mesh();
			Mesh(BatchManager3D& batchManager);
			~Mesh();

			// todo other set mesh methods
			void setMesh(const std::string& _filepath);

			// Setters
			void setPosition(const float _x, const float _y, const float _z);
			void setPosition(const glm::vec3& _newPosition);
			void setPosition(const Mesh& _newPosition);
			void setRotation(const float _yaw, const float _pitch, const float _roll);
			void setRotation(const glm::vec3& _newRotation);
			void setScale(const float _newScale);
			void setScale(const float _newScaleX, const float _newScaleY, const float _newScaleZ);
			void setScale(const glm::vec3& _newScale);
			void setColor(const Mesh& _other);
			void setColor(const Color _color);
			void setAlpha(const float _alpha);
			void translate(const float _x, const float _y, const float _z);
			void translate(const glm::vec3& _translation);
			void setBlending(const bool _value);
			void setDepthTest(const bool _value);
			void setRenderState(const Mesh& _other);
			void setRenderState(const bool _newState);
			void toggleRenderState();
			void setShaderIndex(const unsigned int _newShaderIndex);
			void setBackFaceCulling(const bool _value);
			TextureData* setTexture(const std::string& _texturePath);
			TextureData* setTexture(const size_t _textureID);
			void setTexture(TextureData* _textureDataPtr);

			// Getters
			glm::vec3 getPosition() const { return position; }
			glm::vec3 getRotation() const { return rotation; }
			glm::vec3 getScale() const { return scale; }
			bool getRenderState() const { return renderState; }
			unsigned int getShaderIndex() const { return shaderIndex; }
			se::Color getColor() const { return color; }
			float getAlpha() const { return color.a; }

			// TODO MAKE PROTECTED (skybox)
			BatchManager3D* batchManager = nullptr;

		protected:
			bool needUpdate = true;

			bool backFaceCulling = true;
			bool renderState = true; // Whether Mesh is rendered or not
			bool blending = false;
			bool depthTest = true;
			unsigned int shaderIndex;
			GLuint textureDataID = 0;
			glm::vec3 position = glm::vec3(0.0f);
			glm::vec3 rotation = glm::vec3(0.0f);
			glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
			Color color = Color(255, 255, 255, 255);

			std::vector<Vertex3D> vertexArray; // Original vertices
			std::vector<GLushort> elementArray; // Indices
		};
	}
}
