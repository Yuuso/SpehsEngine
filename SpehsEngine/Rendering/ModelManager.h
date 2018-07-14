
#pragma once

#include "SpehsEngine\Rendering\Vertex3D.h"

#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include <glm/vec3.hpp>


typedef unsigned short GLushort;


namespace se
{
	namespace rendering
	{
		class Mesh;

		int checkArrayForElements(const std::vector<Vertex3D>& _array, const glm::vec3& _position, const glm::vec2& _textureCoord, const glm::vec3& _normal);

		struct ModelData
		{
			ModelData();
			void loadFromData(std::vector<Vertex3D> &_vertexArray, std::vector<GLushort> &_elementArray);

			std::vector<Vertex3D> vertices;
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

			void loadModel(const std::string& _filepath, Mesh* _mesh);

			void loadOBJ(const std::string& _filepath, Mesh* _mesh);
			void loadOBJ(const size_t& _hash, Mesh* _mesh);
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
}
