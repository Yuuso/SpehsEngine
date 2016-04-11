#pragma once

#include "Vertex.h"
#include "Container.h"

#include <glm/vec3.hpp>

#include <string>
#include <unordered_map>


typedef unsigned short GLushort;

namespace spehs
{
	class Mesh;

	int checkArrayForElements(const spehs::Container<spehs::Vertex3D>& _array, const spehs::Position& _position, const glm::vec2& _textureCoord, const glm::vec3& _normal);

	struct ModelData
	{
		ModelData();
		void loadFromData(spehs::Container<spehs::Vertex3D> &_vertexArray, spehs::Container<GLushort> &_elementArray);

		spehs::Container<spehs::Vertex3D> vertices;
		spehs::Container<glm::vec2> textureCoordinates;
		spehs::Container<glm::vec3> normals;
		spehs::Container<GLushort> vertexElements;
		spehs::Container<GLushort> textureElements;
		spehs::Container<GLushort> normalElements;
	};

	class ModelManager
	{
	public:
		ModelManager();
		~ModelManager();

		void loadOBJ(const std::string& _filepath, spehs::Mesh* _mesh);
		void loadOBJ(const size_t& _hash, spehs::Mesh* _mesh);
		void preloadOBJ(const std::string& _filepath);

		void removeModelData(const std::string& _filepath);
		void removeModelData(const size_t& _hash);
		void clearAllModelData();

	private:
		std::unordered_map<size_t, ModelData*> modelDataMap;
	};
}
extern spehs::ModelManager* modelManager;