#pragma once

#include "BatchObject.h"
#include "Vertex.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <vector>
#include <string>


typedef unsigned short GLushort;

namespace SpehsEngine
{
	class Mesh : public BatchObject
	{
	public:
		Mesh();
		Mesh(const std::string &_filepath);

		Mesh* getMeshPtr(){ return this; }

		void updateVertices();

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
		void setColor(const unsigned char &_r, const unsigned char &_g, const unsigned char &_b, const unsigned char &_a = 1.0f);

	protected:
		~Mesh();

		float yaw, pitch, roll; //Rotations
		float scaleX, scaleY, scaleZ;

		std::vector<GLushort> elementArray; //Indices
		glm::vec3* normalArray;
		glm::vec4 color;
	};
}
