
#pragma once

#include "SpehsEngine/Rendering/Vertex3D.h"
#include "SpehsEngine/Core/Vector.h"
#include "SpehsEngine/Core/Color.h"

#include <glm/mat4x4.hpp>

#include <string>
#include <vector>


typedef unsigned short GLushort;
typedef unsigned int GLuint;


namespace spehs
{
	class MeshBatch;
	class ModelManager;
	class TextureData;

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
		void setPosition(const spehs::vec3& _newPosition);
		void setPosition(const Mesh& _newPosition);
		void setRotation(const float _yaw, const float _pitch, const float _roll);
		void setRotation(const spehs::vec3& _newRotation);
		void setScale(const float _newScale);
		void setScale(const float _newScaleX, const float _newScaleY, const float _newScaleZ);
		void setScale(const spehs::vec3& _newScale);
		void setColor(const Mesh& _other);
		void setColor(const Color _color);
		void translate(const float _x, const float _y, const float _z);
		void translate(const spehs::vec3& _translation);
		void setBlending (const bool _value);
		void setDepthTest (const bool _value);
		void setRenderState(const Mesh& _other);
		void setRenderState(const bool _newState);
		void setShader(const int _newShaderIndex);
		void setBackFaceCulling(const bool _value);
		TextureData* setTexture(const std::string& _texturePath);
		TextureData* setTexture(const size_t _textureID);
		void setTexture(TextureData* _textureDataPtr);

		// Getters
		spehs::vec3 getPosition() const { return position; }
		spehs::vec3 getRotation() const { return rotation; }
		spehs::vec3 getScale() const { return scale; }
		bool getRenderState() const { return renderState; }
		int getShaderIndex() const { return shaderIndex; }
		spehs::Color getColor() const { return color; }
		spehs::Color::Component getAlpha() const { return color.a; }

		// Public Variables
		std::vector<spehs::Vertex3D> worldVertexArray; // Transformed vertices
		BatchManager3D& batchManager;

	protected:
		Mesh(BatchManager3D& _batchManager);
		Mesh(BatchManager3D& batchManager, const std::string& _filepath);
		~Mesh();

		bool backFaceCulling;
		bool readyForDelete;
		bool renderState; // Whether Mesh is rendered or not
		bool needUpdate;
		bool blending;
		bool depthTest;
		int shaderIndex;
		GLuint textureDataID;
		spehs::vec3 position;
		spehs::vec3 rotation;
		spehs::vec3 scale;
		Color color;

		glm::mat4 scaledMatrix;
		glm::mat4 scaledRotatedMatrix;
		glm::mat4 transformMatrix;
		glm::mat4 normalMatrix;

		std::vector<spehs::Vertex3D> vertexArray; // Original vertices
		std::vector<GLushort> elementArray; // Indices
	};
}
