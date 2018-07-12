
#pragma once

#include "SpehsEngine\Rendering\Vertex3D.h"
#include "SpehsEngine\Core\Vector.h"

#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>


typedef unsigned short GLushort;


namespace spehs
{
	class Mesh;

	int checkArrayForElements(const std::vector<spehs::Vertex3D>& _array, const spehs::vec3& _position, const spehs::vec2& _textureCoord, const spehs::vec3& _normal);

	struct ModelData
	{
		ModelData();
		void loadFromData(std::vector<spehs::Vertex3D> &_vertexArray, std::vector<GLushort> &_elementArray);

		std::vector<spehs::Vertex3D> vertices;
		std::vector<spehs::vec2> textureCoordinates;
		std::vector<spehs::vec3> normals;
		std::vector<GLushort> vertexElements;
		std::vector<GLushort> textureElements;
		std::vector<GLushort> normalElements;
	};

	class ModelManager
	{
	public:
		ModelManager();
		~ModelManager();

		void loadModel(const std::string& _filepath, spehs::Mesh* _mesh);

		void loadOBJ(const std::string& _filepath, spehs::Mesh* _mesh);
		void loadOBJ(const size_t& _hash, spehs::Mesh* _mesh);
		void preloadOBJ(const std::string& _filepath);

		void removeModelData(const std::string& _filepath);
		void removeModelData(const size_t& _hash);
		void clearAllModelData();

	private:
		std::istringstream loadCube();
		std::istringstream loadInvertedCube();
		std::istringstream loadInvertedSphere();

		std::unordered_map<size_t, ModelData*> modelDataMap;

		void loadOBJFromFile(std::ifstream& _data, const size_t& _hash);
		void loadOBJFromStringStream(std::istringstream& _data, const size_t& _hash);
	};
}