#pragma once

#include "Vertex.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <vector>
#include <string>


typedef unsigned short GLushort;
typedef unsigned int GLuint;

namespace spehs
{
	class MeshBatch;

	class Mesh
	{
		friend class BatchManager;

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
		void setColor(const glm::vec4 &_newColor);//TODO: FIX COLORS
		void setColor(const glm::vec3 &_newColor);
		void setColor(const unsigned char &_r, const unsigned char &_g, const unsigned char &_b, const unsigned char &_a = 1.0f);
		void setRenderState(const bool _newState);
		void setShader(const int &_newShaderIndex);

		//Getters
		int getShaderIndex(){ return shaderIndex; }

		//Public Variables
		Vertex* worldVertexArray; //Transformed vertices
		GLuint numVertices;

	protected:
		Mesh();
		Mesh(const std::string &_filepath);
		~Mesh();

		bool readyForDelete;
		bool renderState; //Whether BatchObject is rendered or not
		bool useColor; //When true: Colors are sent as uniform
		int shaderIndex;
		GLuint textureDataID;
		float yaw, pitch, roll; //Rotations
		float scaleX, scaleY, scaleZ;

		glm::vec4 color;
		glm::mat4 scaledMatrix;
		glm::mat4 scaledRotatedMatrix;

		Position position;
		Vertex* vertexArray; //Original vertices
		glm::vec3* normalArray;
		std::vector<GLushort> elementArray; //Indices
	};
}
