
#pragma once

#include "SpehsEngine\Rendering\Vertex3D.h"
#include "SpehsEngine\Core\Vector.h"

#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>


typedef unsigned short GLushort;


namespace se
{
	namespace rendering
	{
		class Mesh;

		int checkArrayForElements(const std::vector<Vertex3D>& _array, const se::vec3& _position, const se::vec2& _textureCoord, const se::vec3& _normal);

		struct ModelData
		{
			ModelData();
			void loadFromData(std::vector<Vertex3D> &_vertexArray, std::vector<GLushort> &_elementArray);

			std::vector<Vertex3D> vertices;
			std::vector<se::vec2> textureCoordinates;
			std::vector<se::vec3> normals;
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
