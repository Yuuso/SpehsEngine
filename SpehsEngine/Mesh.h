#pragma once

#include "Vertex.h"
#include "Primitive.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>


typedef unsigned short GLushort;
typedef unsigned int GLuint;

namespace spehs
{
	class MeshBatch;
	class ModelManager;
	struct TextureData;

	class Mesh
	{
		friend class BatchManager;
		friend class MeshBatch;
		friend class ModelManager;

	public:
		void updateVertices(); //This is called automatically when rendering

		void destroy(); //Meshes can only be deleted from BatchManager, user can request deletion by calling destroy()

		void setMesh(const std::string &_filepath);

		//Setters
		void setPosition(const float &_x, const float &_y, const float &_z);
		void setPosition(const glm::vec3 &_newPosition);
		void setPosition(const Position &_newPosition);
		void setRotation(const float &_yaw, const float &_pitch, const float &_roll);
		void setRotation(const glm::vec3 &_newRotation);
		void setScale(const float &_newScale);
		void setScale(const float &_newScaleX, const float &_newScaleY, const float &_newScaleZ);
		void setScale(const glm::vec3 &_newScale);
		void setColor(const glm::vec4 &_newColor);
		void setColor(const glm::vec3 &_newColor);
		void setColor(const CommonColor& _color);
		void setColor(const unsigned char &_r, const unsigned char &_g, const unsigned char &_b, const unsigned char &_a = 255);
		void setRenderState(const bool _newState);
		void setShader(const int &_newShaderIndex);
		void setDrawMode(const DrawMode &_drawMode);
		void setLineWidth(const float &_lineWidth);
		void setBlending(const bool &_value);
		void setBackFaceCulling(const bool& _value);
		TextureData* setTexture(const std::string &_texturePath);
		TextureData* setTexture(const size_t &_textureID);
		void setTexture(TextureData* _textureDataPtr);

		//Getters
		int getShaderIndex(){ return shaderIndex; }
		bool getRenderState(){ return renderState; }
		glm::vec4 getColor(){ return color; }
		glm::vec3 getPosition(){ return spehs::toVec3(position); }
		glm::vec3 getRotation(){ return rotation; }


		//Public Variables
		std::vector<spehs::Vertex3D> worldVertexArray; //Transformed vertices
		std::vector<spehs::Vertex3D> vertexArray; //Original vertices

	protected:
		Mesh();
		Mesh(const std::string &_filepath);
		~Mesh();

		bool backFaceCulling;
		bool readyForDelete;
		bool renderState; //Whether Mesh is rendered or not
		bool needUpdate;
		bool blending;
		int shaderIndex;
		GLuint textureDataID;
		DrawMode drawMode;
		float lineWidth;
		glm::vec3 rotation;
		float scaleX, scaleY, scaleZ;

		glm::vec4 color;
		glm::mat4 scaledMatrix;
		glm::mat4 scaledRotatedMatrix;
		glm::mat4 transformMatrix;
		glm::mat4 normalMatrix;

		spehs::Position position;
		std::vector<GLushort> elementArray; //Indices
	};
}
