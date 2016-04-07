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

	int checkArrayForElements(std::vector<spehs::Vertex3D>& _array, const spehs::Position& _position, const glm::vec2& _textureCoord, const glm::vec3& _normal);

	struct ModelData
	{
		ModelData();
		void loadFromData(std::vector<spehs::Vertex3D> &_vertexArray, std::vector<GLushort> &_elementArray);

		std::vector<spehs::Vertex3D> vertices;
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