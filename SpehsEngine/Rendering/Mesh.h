
#pragma once

#include "SpehsEngine/Rendering/Vertex3D.h"
#include "SpehsEngine/Core/Vector.h"
#include "SpehsEngine/Core/Color.h"

#include <glm/mat4x4.hpp>

#include <string>
#include <vector>


typedef unsigned short GLushort;
typedef unsigned int GLuint;


namespace se
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
		void updateVertices(); // This is called automatically when rendering

		void destroy(); // Meshes can only be deleted from BatchManager, user can request deletion by calling destroy()

		void setMesh(const std::string& _filepath);

		// Setters
		void setPosition(const float _x, const float _y, const float _z);
		void setPosition(const se::vec3& _newPosition);
		void setPosition(const Mesh& _newPosition);
		void setRotation(const float _yaw, const float _pitch, const float _roll);
		void setRotation(const se::vec3& _newRotation);
		void setScale(const float _newScale);
		void setScale(const float _newScaleX, const float _newScaleY, const float _newScaleZ);
		void setScale(const se::vec3& _newScale);
		void setColor(const Mesh& _other);
		void setColor(const Color _color);
		void setAlpha(const float _alpha);
		void translate(const float _x, const float _y, const float _z);
		void translate(const se::vec3& _translation);
		void setBlending (const bool _value);
		void setDepthTest (const bool _value);
		void setRenderState(const Mesh& _other);
		void setRenderState(const bool _newState);
		void setShaderIndex(const unsigned int _newShaderIndex);
		void setBackFaceCulling(const bool _value);
		TextureData* setTexture(const std::string& _texturePath);
		TextureData* setTexture(const size_t _textureID);
		void setTexture(TextureData* _textureDataPtr);

		// Getters
		se::vec3 getPosition() const { return position; }
		se::vec3 getRotation() const { return rotation; }
		se::vec3 getScale() const { return scale; }
		bool getRenderState() const { return renderState; }
		unsigned int getShaderIndex() const { return shaderIndex; }
		se::Color getColor() const { return color; }
		float getAlpha() const { return color.a; }

		// Public Variables
		std::vector<se::Vertex3D> worldVertexArray; // Transformed vertices
		BatchManager3D& batchManager;

	protected:
		Mesh(BatchManager3D& _batchManager);
		Mesh(BatchManager3D& batchManager, const std::string& _filepath);
		~Mesh();

		bool backFaceCulling = true;
		bool readyForDelete = false;
		bool renderState = true; // Whether Mesh is rendered or not
		bool needUpdate = true;
		bool blending = false;
		bool depthTest = true;
		unsigned int shaderIndex;
		GLuint textureDataID = 0;
		se::vec3 position = se::vec3::zero;
		se::vec3 rotation = se::vec3::zero;
		se::vec3 scale = se::vec3(1.0f, 1.0f, 1.0f);
		Color color = Color(255, 255, 255, 255);

		glm::mat4 scaledMatrix = glm::mat4(1.0f);
		glm::mat4 scaledRotatedMatrix = glm::mat4(1.0f);
		glm::mat4 transformMatrix = glm::mat4(1.0f);
		glm::mat4 normalMatrix = glm::mat4(1.0f);

		std::vector<se::Vertex3D> vertexArray; // Original vertices
		std::vector<GLushort> elementArray; // Indices
	};
}
