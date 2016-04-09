#pragma once

#include "Vertex.h"
#include "Primitive.h"
#include "Container.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>


typedef unsigned short GLushort;
typedef unsigned int GLuint;

namespace spehs
{
	class MeshBatch;
	class ModelManager;

	class Mesh
	{
		friend class BatchManager;
		friend class MeshBatch;
		friend class ModelManager;

	public:
		void updateVertices(); //This is called automatically when rendering

		void destroy(); //Meshes can only be deleted from BatchManager, user can request deletion by calling destroy()

		void setOBJMesh(const std::string &_objFilepath);

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

		//Getters
		int getShaderIndex(){ return shaderIndex; }
		bool getRenderState(){ return renderState; }
		glm::vec4 getColor(){ return color; }


		//Public Variables
		spehs::Container<spehs::Vertex3D> worldVertexArray; //Transformed vertices

	protected:
		Mesh();
		Mesh(const std::string &_filepath);
		~Mesh();

		bool readyForDelete;
		bool renderState; //Whether Mesh is rendered or not
		bool needUpdate;
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

		spehs::Position position;
		spehs::Container<spehs::Vertex3D> vertexArray; //Original vertices
		spehs::Container<GLushort> elementArray; //Indices
	};
}
