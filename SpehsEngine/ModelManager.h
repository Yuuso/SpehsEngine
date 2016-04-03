#pragma once

#include "Vertex.h"

#include <glm/vec3.hpp>

#include <string>
#include <vector>
#include <unordered_map>


typedef unsigned short GLushort;

namespace spehs
{
	class Mesh;

	struct ModelData
	{
		ModelData();
		void loadFromData(spehs::Vertex* _vertexArray, unsigned int* _numVertices, std::vector<GLushort> &_elementArray, std::vector<glm::vec3> &_normalArray);

		std::vector<spehs::Vertex> vertices;
		std::vector<glm::vec2> textureCoordinates;
		std::vector<glm::vec3> normals;
		std::vector<GLushort> vertexElements;
		std::vector<GLushort> textureElements;
		std::vector<GLushort> normalElements;
	};

	class ModelManager
	{
	public:
		ModelManager();
		~ModelManager();

		void loadOBJ(const std::string& _filepath, spehs::Mesh* _mesh);
		void loadOBJ(const size_t& _hash, spehs::Mesh* _mesh);
		void preloadOBJ(const std::string& _filepath);

		void removeModelData(std::string _filepath);
		void removeModelData(size_t _hash);
		void clearAllModelData();

	private:
		std::unordered_map<size_t, ModelData*> modelDataMap;
	};
}
extern spehs::ModelManager* modelManager;